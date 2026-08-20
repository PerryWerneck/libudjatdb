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
  * @brief Internal definitions for CPPDB module.
  */

 #pragma once
 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/variant.h>
 #include <cppdb/frontend.h>
 #include <mutex>

 namespace Udjat {

	namespace SQL {

		class UDJAT_API Session : public cppdb::session {
		private:
			// void get(cppdb::result &rc, Udjat::Variant &value);

		public:

			Session(const char *dbname);
			~Session();

			// static void exec(SQL::Session &session, Udjat::String statement, const Udjat::Variant &request, Udjat::Variant &response, const char *child_name = nullptr);
			// void exec(Udjat::String statement, const Udjat::Variant &request, Udjat::Variant &response, const char *child_name = nullptr);

			// sqlite3_stmt * prepare(String &statement, const Udjat::Variant &request, const Udjat::Variant &response);
			// void exec(sqlite3_stmt *statement, Udjat::Variant &response);

		};

	}

 }

