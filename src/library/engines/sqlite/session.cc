/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) 2024 Perry Werneck <perry.werneck@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

 /**
  * @brief Implements sqlite3 session.
  */

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/string.h>
 #include <udjat/tools/value.h>
 #include <udjat/tools/report.h>
 #include <udjat/tools/logger.h>
 #include <udjat/tools/sql.h>
 #include <private/sqlite.h>
 #include <sqlite3.h>
 #include <mutex>
 #include <stdexcept>

 using namespace std;

 namespace Udjat {

	std::mutex SQL::Session::guard;

	SQL::Session::Session(const char *dbname) {

		lock_guard<mutex> lock(guard);
		Logger::String{"Opening database on '",dbname,"'"}.trace("sqlite");

		// Open database.
		int rc = sqlite3_open(dbname, &db);
		if(rc != SQLITE_OK) {
			db = nullptr;
			throw runtime_error(Logger::String{"Error opening '",dbname,"'"});
		}

	}

	SQL::Session::~Session() {

		lock_guard<std::mutex> lock(guard);

		if(db) {
			switch(sqlite3_close(db)) {
			case SQLITE_OK:
				Logger::String{"Closing database with NO unfinished operations"}.trace("sqlite");
				break;

			case SQLITE_BUSY:
				Logger::String{"Closing database with unfinished operations"}.warning("sqlite");
				break;

			default:
				Logger::String{"Unexpected error closing database"}.error("sqlite");
			}
			db = nullptr;
		}

	}

	void SQL::Session::check(int rc) const {
		if (rc != SQLITE_OK && rc != SQLITE_DONE) {
			debug("rc=",rc," error ",sqlite3_errmsg(db));
			throw runtime_error(sqlite3_errmsg(db));
		}
	}

	void SQL::Session::get(sqlite3_stmt *stmt, Udjat::Report &report) {
		int colnum = sqlite3_data_count(stmt);

		for(int col = 0; col < colnum;col++) {

			const char *name = sqlite3_column_name(stmt,col);
			debug(name,"='",(const char *) sqlite3_column_text(stmt,col),"'");

			switch(sqlite3_column_type(stmt,col)) {
			case SQLITE_INTEGER:
				report.push_back(sqlite3_column_int(stmt,col));
				break;

			case SQLITE_FLOAT:
				report.push_back(sqlite3_column_double(stmt,col));
				break;

			case SQLITE_BLOB:
				throw runtime_error(Logger::String{"Unsupported 'blob' column ",name});
				break;

			case SQLITE_NULL:
				report.push_back("");
				break;

			default:
				// Unknown, assume as text.
				report.push_back((const char *) sqlite3_column_text(stmt,col));
			}
		}

	}

	void SQL::Session::get(sqlite3_stmt *stmt, Udjat::Value &value) {

		int colnum = sqlite3_data_count(stmt);

		for(int col = 0; col < colnum;col++) {
			const char *name = sqlite3_column_name(stmt,col);
			debug(name,"='",(const char *) sqlite3_column_text(stmt,col));

			switch(sqlite3_column_type(stmt,col)) {
			case SQLITE_INTEGER:
				value[name] = sqlite3_column_int(stmt,col);
				break;

			case SQLITE_FLOAT:
				value[name] = sqlite3_column_double(stmt,col);
				break;

			case SQLITE_TEXT:
				value[name] = (const char *) sqlite3_column_text(stmt,col);
				break;

			case SQLITE_BLOB:
				throw runtime_error(Logger::String{"Unsupported 'blob' column ",name});
				break;

			case SQLITE_NULL:
				value[name] = "";
				break;

			default:
				Logger::String{"Unexpected data type in column '",name,"', assuming string"}.warning("sqlite");
				value[name] = (const char *) sqlite3_column_text(stmt,col);
			}
		}

	}

	static const Value & get_value(const Udjat::Value &request, const Udjat::Value &response, const char *name) {

		if(request.contains(name)) {
			return request[name];
		}

		if(response.contains(name)) {
			return response[name];
		}

		throw runtime_error(Logger::String{"Cant find required property '",name,"'"});

	}

	sqlite3_stmt * SQL::Session::prepare(Udjat::String &line, const Udjat::Value &request, const Udjat::Value &response) {

		vector<string> names;

		line.expand([&](const char *key, std::string &value){
			names.push_back(key);
			value = "?";
			return true;
		},false,true);

		debug(line.c_str());

		sqlite3_stmt *stmt;
		check(
			sqlite3_prepare_v2(
				db,           	// Database handle
				line.c_str(),	// SQL statement, UTF-8 encoded
				-1,             // Maximum length of zSql in bytes.
				&stmt,          // OUT: Statement handle
				NULL            // OUT: Pointer to unused portion of zSql
			)
		);

		//
		// Bind value contents.
		//
		int column = 1;
		for(string &name : names) {

			const Value &value = get_value(request, response, name.c_str());

			switch((Value::Type) value) {
			case Value::Undefined:
				sqlite3_bind_null(stmt,column);
				break;

			case Value::String:
			case Value::Icon:
			case Value::Url:
				debug("value(",column,",'",name,"')='",value.c_str(),"' (from request)");
				sqlite3_bind_text(
					stmt,
					column,
					value.c_str(),
					strlen(value.c_str())+1,
					SQLITE_TRANSIENT
				);
				break;

			case Value::Timestamp:
				{
					time_t v;
					value.get(v);
					sqlite3_bind_int64(stmt,column,(sqlite3_int64) v);
				}
				break;

			case Value::Signed:
			case Value::Boolean:
			case Value::State:
				{
					int v;
					value.get(v);
					sqlite3_bind_int(stmt,column,v);
				}
				break;

			case Value::Unsigned:
				{
					unsigned int v;
					value.get(v);
					sqlite3_bind_int(stmt,column,v);
				}
				break;

			case Value::Real:
				{
					double v;
					value.get(v);
					sqlite3_bind_double(stmt,column,v);
				}
				break;

			case Value::Fraction:
				{
					double v;
					value.get(v);
					sqlite3_bind_double(stmt,column,v*100);
				}
				break;

			default:
				throw logic_error(Logger::Message("Unexpected type binding '{}'",name));
			}
			column++;
		}

		return stmt;

	}

 }

