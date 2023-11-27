/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) 2023 Perry Werneck <perry.werneck@gmail.com>
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
  * @brief Declare abstract SQL Query.
  */

 #pragma once

 #include <udjat/defs.h>
 #include <udjat/tools/xml.h>
 #include <udjat/tools/object.h>
 #include <udjat/tools/value.h>
 #include <cppdb/frontend.h>
 #include <vector>
 #include <memory>

 namespace Udjat {

	namespace SQL {

		using Session = cppdb::session;

		/// @brief An SQL statement.
		class UDJAT_API Statement {
		private:

			/// @brief The database URL;
			const char *dburl = nullptr;

			/// @brief An SQL script.
			struct Script {
				const char *text;
				std::vector<const char *> parameter_names;
				Script(const char *script);
				void exec(Session &session, const Udjat::Object &object);
				void exec(Session &session, const Udjat::Object &request, Udjat::Value &response);
			};

			std::vector<Script> scripts;

		public:

			Statement(const XML::Node &node);

			virtual ~Statement();

			/// @brief Execute query without any parameters.
			void exec();

			/// @brief Execute SQL query
			/// @param request The object with the parameter values.
			void exec(const Udjat::Object &request);

			/// @brief Execute SQL query, get response.
			void exec(const Udjat::Object &request, Udjat::Value &response);

			/// @brief Execute SQL query.
			static void exec(const XML::Node &node);


		};

	}

 }

