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
 #include <udjat/tools/value.h>
 #include <cppdb/frontend.h>
 #include <mutex>

 namespace Udjat {

	namespace SQL {

		class UDJAT_API Session : public cppdb::session {
		private:
			void get(cppdb::result &rc, Udjat::Value &value);
			void get(cppdb::result &rc, Udjat::Report &report);

		public:

			Session(const char *dbname);
			~Session();

			static void exec(SQL::Session &session, Udjat::String statement, const Udjat::Value &request, Udjat::Value &response, const char *child_name = nullptr);
			void exec(Udjat::String statement, const Udjat::Value &request, Udjat::Value &response, const char *child_name = nullptr);

		};

	}

 }

/*
 #pragma once

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/sql/script.h>
 #include <udjat/tools/value.h>
 #include <udjat/tools/abstract/object.h>
 #include <cppdb/frontend.h>
 #include <mutex>

 namespace Udjat {

	namespace SQL {

		void bind(const SQL::Statement &script, cppdb::statement &stmt, const Abstract::Object &request, Udjat::Value &response);
		void exec(cppdb::session &session, const std::vector<SQL::Statement> &scripts, const Abstract::Object &request, Udjat::Value &response);
		void parse_result(cppdb::result &res, Udjat::Value &response);

	}

 }
*/
