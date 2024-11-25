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
  * @brief Execute SQLite script.
  */

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/string.h>
 #include <udjat/tools/sql/script.h>
 #include <udjat/tools/value.h>
 #include <string>
 #include <private/sqlite.h>
 #include <sqlite3.h>
 #include <vector>

 using namespace std;

 namespace Udjat {

	void SQL::Script::exec(const char *dbname, Udjat::Value &values) const {
		exec(dbname,values,values);
	}

	void SQL::Script::exec(const char *dbname, const Udjat::Value &request, Udjat::Value &response) const {
		Session{dbname}.exec(sql,request,response);
	}

	void SQL::Session::exec(Udjat::String statement, const Udjat::Value &request, Udjat::Value &response, const char *name) {

		for(String &line : statement.split(";")) {

			sqlite3_stmt *stmt = prepare(line,request,response);

			switch(sqlite3_step(stmt)) {
			case SQLITE_DONE:
				debug("Empty response");
				continue;

			case SQLITE_ROW:
				{
					// Parse first line.
					Value row;
					get(stmt,row);

					// Check if have more lines.
					if(sqlite3_step(stmt) == SQLITE_ROW) {

						Value &repoval = response;
						if(name) {
							repoval = response[name];
						} else {
							repoval.clear();
						}
						
						auto &report = repoval.ReportFactory(row);

						do {
							get(stmt,report);
						} while(sqlite3_step(stmt) == SQLITE_ROW);

					} else {

						response.merge(row);

					}
				}
				break;

			default:
				throw runtime_error(sqlite3_errmsg(db));

			}


		}

	}

 }


/*
 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/sql/script.h>
 #include <udjat/tools/value.h>
 #include <string>
 #include <sqlite3.h>
 #include <private/sqlite.h>

 using namespace std;

 namespace Udjat {

	UDJAT_API const char * SQL::engine() noexcept {
		return "sqlite";
	}

	void SQL::Script::exec(const Udjat::Object &request) const {

		debug(__FUNCTION__);

		auto values = Udjat::Value::ObjectFactory();
		SQL::Session{dburl}.exec(scripts,request,*values);

	}

	void SQL::Script::exec(std::shared_ptr<Udjat::Value> response) const {

		debug(__FUNCTION__);
		SQL::Session{dburl}.exec(scripts,*response);

	}

	void SQL::Script::exec(const Udjat::Object &request, Udjat::Value &response) const {

		debug(__FUNCTION__);
		SQL::Session{dburl}.exec(scripts,request,response);

	}

	void SQL::Script::exec(const Request &request, Udjat::Value &response) const {

		debug(__FUNCTION__,"::Value start");
		SQL::Session{dburl}.exec(scripts,request,response);
		debug(__FUNCTION__,"::Value ends");

	}

	void SQL::Script::exec(const Request &request, Udjat::Response::Table &response) const {

		debug(__FUNCTION__,"::Table start");
		SQL::Session{dburl}.exec(scripts,request,response);
		debug(__FUNCTION__,"::Table ends");
	}

 }

*/
