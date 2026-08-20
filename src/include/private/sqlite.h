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
  * @brief Private definitions for SQLite3 module.
  */

 #pragma once
 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/variant.h>
 #include <mutex>
 #include <sqlite3.h>

 namespace Udjat {

	namespace SQL {

		class UDJAT_API Session {
		private:
			sqlite3 *db = NULL;
			static std::mutex guard;

			// void get(sqlite3_stmt *stmt, Udjat::Variant &value);

			void check(int rc) const;

			sqlite3_stmt * prepare(String &statement, const Udjat::Variant &request, const Udjat::Variant &response);
			void exec(sqlite3_stmt *statement, Udjat::Variant &response);

		public:

			Session(const char *dbname);
			~Session();

			void exec(Udjat::String statement, const Udjat::Variant &request, Udjat::Variant &response, const char *child_name = nullptr);

		};

	}

 }
