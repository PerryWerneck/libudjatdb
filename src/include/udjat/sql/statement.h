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
 #include <cppdb/frontend.h>
 #include <vector>
 #include <memory>

 namespace Udjat {

	namespace SQL {

		/// @brief An SQL statement.
		class UDJAT_API Statement {
		private:

			/// @brief An SQL query parameter.
			struct Parameter {
				const char *name;
				size_t index;
				constexpr Parameter(const char *n, size_t i) : name{n}, index{i} {
				}

			};

			std::vector<Parameter> parameters;

		public:

			Statement(const char *sql, bool cached = true);
			Statement(const XML::Node &node);

			virtual ~Statement();

			/// @brief Execute query without any parameters.
			void exec();

			/// @brief Execute SQL query
			/// @param request The object with the parameter values.
			void exec(const Udjat::Object &request);

		};

	}

 }

