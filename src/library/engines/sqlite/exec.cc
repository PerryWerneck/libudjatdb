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
 #include <udjat/tools/variant.h>
 #include <string>
 #include <private/sqlite.h>
 #include <sqlite3.h>
 #include <vector>

 using namespace std;

 namespace Udjat {

	void SQL::Script::exec(const char *dbname, Udjat::Variant &values) const {
		exec(dbname,values,values);
	}

	void SQL::Script::exec(const char *dbname, const Udjat::Variant &request, Udjat::Variant &response) const {
		Session{dbname}.exec(sql,request,response);
	}

	static Variant::Type TypeFactory(int sqlite_type) noexcept {

		static const struct {
			int 			sqlite;
			Variant::Type	variant;
		} types[] = {
			{ SQLITE_FLOAT,		Variant::Real	},	// REAL result
			{ SQLITE_INTEGER,	Variant::Signed	},	// 32-bit INTEGER result
			{ SQLITE_TEXT,		Variant::String	}	// UTF-8 TEXT result
		};

		for(const auto &type : types) {
			if(type.sqlite == sqlite_type) {
				return type.variant;
			}
		}

		return Variant::String;

	}

	void SQL::Session::exec(sqlite3_stmt *stmt, Udjat::Variant &response) {

		try {

			switch(sqlite3_step(stmt)) {
			case SQLITE_DONE:
				debug("Empty response");
				response.reset(Variant::Boolean);
				response = true;
				break;

			case SQLITE_ROW:

				{
					// Parse column names & types.
					int colnum = sqlite3_data_count(stmt);
					for(int col = 0; col < colnum;col++) {
						response.add_column(
							sqlite3_column_name(stmt,col),
							TypeFactory(sqlite3_column_type(stmt,col))
						);
					}

					// Parse contents
					do {
						for(int col = 0; col < colnum;col++) {

							switch(sqlite3_column_type(stmt,col)) {
							case SQLITE_INTEGER:
								response.append(
									(int) sqlite3_column_int(stmt,col)
								);
								break;

							case SQLITE_FLOAT:
								response.append(
									(double) sqlite3_column_double(stmt,col)
								);
								break;

							case SQLITE_BLOB:
								throw runtime_error(Logger::String{"Unsupported 'blob' column ",sqlite3_column_name(stmt,col)});
								break;

							case SQLITE_NULL:
								response.append("");
								break;

							default:
								// all others are strings.
								response.append(
									(const char *) sqlite3_column_text(stmt,col)
								);
							}

						}
					} while(sqlite3_step(stmt) == SQLITE_ROW);

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

	void SQL::Session::exec(Udjat::String statement, const Udjat::Variant &request, Udjat::Variant &response, const char *name) {

		if(statement.empty()) {
			throw invalid_argument("Empty SQL statement");
		}

		lock_guard<std::mutex> lock(guard);

		auto rows = statement.split(";");
		if(rows.size() == 1) {
			response.reset(Variant::DataTable);
			exec(prepare(rows[0],request,response),response);
		} else {
			response.reset(Variant::Array);
			for(auto &row : rows) {
				exec(prepare(row,request,response),response.append(Variant::DataTable));
			}
		}

	}

 }

