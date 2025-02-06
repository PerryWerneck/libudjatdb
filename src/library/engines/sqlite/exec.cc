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
 #include <udjat/tools/report.h>
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

		if(statement.empty()) {
			throw invalid_argument("Empty SQL statement");
		}

		lock_guard<std::mutex> lock(guard);

		debug("Statement:\n",statement.c_str());

		for(String &line : statement.split(";")) {

			if(Logger::enabled(Logger::Trace)) {
				Logger::String{line.c_str()}.trace("sql");
			}

			sqlite3_stmt *stmt = prepare(line,request,response);

			try {

				switch(sqlite3_step(stmt)) {
				case SQLITE_DONE:
					debug("Empty response");
					break;

				case SQLITE_ROW:
					{
						// Parse first line.
						Value row;
						get(stmt,row);

						// Check if have more lines.
						if(sqlite3_step(stmt) == SQLITE_ROW) {

							// Got second row, change behavior.

							Value &repoval = response;
							if(name) {
								repoval = response[name];
							} else {
								repoval.clear();
							}

							std::vector<string> names;

							{
								int colnum = sqlite3_data_count(stmt);
								for(int col = 0; col < colnum;col++) {
									names.push_back(sqlite3_column_name(stmt,col));
								}
							}

							auto &report = repoval.ReportFactory(names);
							report.push_back(row);

							do {
								get(stmt,report);
							} while(sqlite3_step(stmt) == SQLITE_ROW);

						} else {

							// No second row, add results do response.
							response.merge(row);

						}
					}
					break;

				default:
					throw runtime_error(sqlite3_errmsg(db));

				}

			} catch(...) {

				debug("Finalizing stmt, failed");
				sqlite3_finalize(stmt);
				throw;

			}

			debug("Finalizing stmt, success");
			sqlite3_finalize(stmt);

		}

	}

 }

