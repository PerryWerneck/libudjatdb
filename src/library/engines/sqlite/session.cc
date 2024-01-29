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
 #include <mutex>
 #include <sqlite3.h>
 #include <private/sqlite3.h>

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


 }
