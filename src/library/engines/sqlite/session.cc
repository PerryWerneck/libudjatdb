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
 #include <udjat/tools/logger.h>
 #include <udjat/tools/sql/script.h>
 #include <mutex>
 #include <sqlite3.h>
 #include <private/sqlite.h>
 #include <mutex>

 using namespace std;

 namespace Udjat {

	std::mutex SQL::Session::guard;

	SQL::Session::Session(const char *dbname) {

		lock_guard<std::mutex> lock(guard);

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

	void SQL::Session::check(int rc) {
		if (rc != SQLITE_OK && rc != SQLITE_DONE) {
			throw runtime_error(sqlite3_errmsg(db));
		}
	}

	sqlite3_stmt * SQL::Session::prepare(const SQL::Statement::_Script &script) {

		debug("Preparing '",script.text,"'");

		sqlite3_stmt *stmt;
		check(
			sqlite3_prepare_v2(
				db,           	// Database handle
				script.text,    // SQL statement, UTF-8 encoded
				-1,             // Maximum length of zSql in bytes.
				&stmt,          // OUT: Statement handle
				NULL            // OUT: Pointer to unused portion of zSql
			)
		);
		return stmt;
	}

	void SQL::Session::bind(const SQL::Statement::_Script &script, sqlite3_stmt *stmt, const Abstract::Object &request, Udjat::Value &response) {

		int column = 1;
		for(const char *name : script.parameter_names) {

			string value;

			if(request.getProperty(name,value)) {

				debug("value(",column,",'",name,"')='",value,"' (from request)");
				check(
					sqlite3_bind_text(
						stmt,
						column,
						value.c_str(),
						value.size()+1,
						SQLITE_TRANSIENT
					)
				);

			} else if(response.getProperty(name,value)) {

				debug("value(",column,",'",name,"')='",value,"' (from response)");
				check(
					sqlite3_bind_text(
						stmt,
						column,
						value.c_str(),
						value.size()+1,
						SQLITE_TRANSIENT
					)
				);

			} else {

				throw runtime_error(Logger::String{"Required property '",name,"' is missing"});

			}
			column++;

		}

	}

	void SQL::Session::bind(const SQL::Statement::_Script &script, sqlite3_stmt *stmt, Udjat::Value &response) {

		int column = 1;
		for(const char *name : script.parameter_names) {

			string value;

			if(response.getProperty(name,value)) {

				debug("value(",column,",'",name,"')='",value,"' (from request)");
				check(
					sqlite3_bind_text(
						stmt,
						column,
						value.c_str(),
						value.size()+1,
						SQLITE_TRANSIENT
					)
				);

			} else {

				throw runtime_error(Logger::String{"Required property '",name,"' is missing"});

			}
			column++;

		}

	}

	void SQL::Session::get(sqlite3_stmt *stmt, Udjat::Value &response) {

		int colnum = sqlite3_data_count(stmt);

		for(int col = 0; col < colnum;col++) {
			const char *name = sqlite3_column_name(stmt,col);
			debug(name,"='",(const char *) sqlite3_column_text(stmt,col));

			switch(sqlite3_column_type(stmt,col)) {
			case SQLITE_INTEGER:
				response[name] = sqlite3_column_int(stmt,col);
				break;

			case SQLITE_FLOAT:
				response[name] = sqlite3_column_double(stmt,col);
				break;

			case SQLITE_TEXT:
				response[name] = (const char *) sqlite3_column_text(stmt,col);
				break;

			case SQLITE_BLOB:
				throw runtime_error(Logger::String{"Unsupported 'blob' column ",name});
				break;

			case SQLITE_NULL:
				response[name] = "";
				break;

			default:
				Logger::String{"Unexpected data type in column '",name,"', assuming string"}.warning("sqlite");
				response[name] = (const char *) sqlite3_column_text(stmt,col);
			}
		}

	}

	void SQL::Session::get(sqlite3_stmt *stmt, Udjat::Response::Table &response) {

		int colnum = sqlite3_data_count(stmt);

		for(int col = 0; col < colnum;col++) {

			switch(sqlite3_column_type(stmt,col)) {
			case SQLITE_INTEGER:
				response.push_back(sqlite3_column_int(stmt,col));
				break;

			case SQLITE_FLOAT:
				response.push_back(sqlite3_column_double(stmt,col));
				break;

			case SQLITE_TEXT:
				response.push_back((const char *) sqlite3_column_text(stmt,col));
				break;

			case SQLITE_BLOB:
				throw runtime_error(Logger::String{"Unsupported 'blob' column ",sqlite3_column_name(stmt,col)});
				break;

			case SQLITE_NULL:
				response.push_back("");
				break;

			default:
				Logger::String{"Unexpected data type in column '",sqlite3_column_name(stmt,col),"', assuming string"}.warning("sqlite");
				response.push_back((const char *) sqlite3_column_text(stmt,col));
			}

		}

	}

	int SQL::Session::step(sqlite3_stmt *stmt, Udjat::Value &response) {

		int state = sqlite3_step(stmt);
		switch(state) {
		case SQLITE_DONE:	// Executed, no row
			debug("sqlite-done");
			break;

		case SQLITE_ROW:	// Got a row.
			get(stmt,response);
			break;

		default:
			throw runtime_error(sqlite3_errmsg(db));

		}

		return state;
	}

	void SQL::Session::exec(const std::vector<SQL::Statement::_Script> &scripts, const Abstract::Object &request, Udjat::Value &response) {

		debug(__FUNCTION__);
		lock_guard<std::mutex> lock(guard);

		for(auto &script : scripts) {
			if(script.text && *script.text) {
				sqlite3_stmt *stmt = prepare(script);
				try {

					bind(script, stmt, request, response);
					step(stmt, response);

				} catch(...) {
					sqlite3_finalize(stmt);
					throw;
				}
				sqlite3_finalize(stmt);
			}
		}

	}

	void SQL::Session::exec(const std::vector<SQL::Statement::_Script> &scripts, Udjat::Value &response) {

		debug(__FUNCTION__);
		lock_guard<std::mutex> lock(guard);

		for(auto &script : scripts) {
			if(script.text && *script.text) {
				sqlite3_stmt *stmt = prepare(script);
				try {

					bind(script, stmt, response);
					step(stmt, response);

				} catch(...) {
					sqlite3_finalize(stmt);
					throw;
				}
				sqlite3_finalize(stmt);
			}
		}


	}

	void SQL::Session::exec(const std::vector<SQL::Statement::_Script> &scripts, const Request &request, Udjat::Value &response) {

		debug(__FUNCTION__);
		lock_guard<std::mutex> lock(guard);

		for(auto &script : scripts) {
			if(script.text && *script.text) {
				sqlite3_stmt *stmt = prepare(script);
				try {

					bind(script, stmt, request, response);
					step(stmt, response);

				} catch(...) {
					sqlite3_finalize(stmt);
					throw;
				}
				sqlite3_finalize(stmt);
			}
		}

	}

	void SQL::Session::exec(const std::vector<SQL::Statement::_Script> &scripts, const Request &request, Udjat::Response::Table &response) {

		debug(__FUNCTION__);
		lock_guard<std::mutex> lock(guard);

		for(auto &script : scripts) {
			if(script.text && *script.text) {
				sqlite3_stmt *stmt = prepare(script);
				try {

					{
						int column = 1;
						for(const char *name : script.parameter_names) {

							string value;

							if(request.getProperty(name,value)) {

								debug("value(",name,")='",value,"' (from request)");
								check(
									sqlite3_bind_text(
										stmt,
										column,
										value.c_str(),
										value.size()+1,
										SQLITE_TRANSIENT
									)
								);

							} else {

								throw runtime_error(Logger::String{"Required property '",name,"' is missing"});

							}
							column++;

						}
					}

					int state = sqlite3_step(stmt);
					switch(state) {
					case SQLITE_DONE:	// Executed, no row
						break;

					case SQLITE_ROW:	// Got a row.
						{
							int numcols = sqlite3_data_count(stmt);
							std::vector<string> colnames;

							for(int col = 0; col < numcols;col++) {
								colnames.push_back(sqlite3_column_name(stmt,col));
							}

							// Start report...
							response.start(colnames);

							// Get first row.
							get(stmt,response);

							while(sqlite3_step(stmt) == SQLITE_ROW) {
								get(stmt,response);
							}

						}
						break;

					default:
						throw runtime_error(sqlite3_errmsg(db));

					}

				} catch(...) {
					sqlite3_finalize(stmt);
					throw;
				}
				sqlite3_finalize(stmt);
			}
		}

	}

 }
