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
 #include <udjat/tools/properties.h>
 #include <udjat/tools/variant.h>
 #include <udjat/tools/string.h>
 #include <udjat/tools/script.h>
 #include <string>
 #include <vector>
 #include <memory>

 namespace Udjat {

	namespace SQL {

		/// @brief Get engine name
		/// @return The engine name ("sqlite", "cppdb", ...)
		UDJAT_API const char * engine() noexcept;

		class UDJAT_API Script {
		private:
			String sql;	///< @brief The string with SQL Queries.

		protected:

		public:

			/// @brief Create SQL statement from XML definition.
			/// @param sql the script definition.
			Script(const char *sql);

			Script(const std::string &str) : Script{str.c_str()} {
			}

			inline const char *c_str() const noexcept {
				return sql.c_str();
			}

			/// @brief Create SQL statement from XML definition.
			/// @param node the script definition.
			Script(const Properties &props);

			/// @brief Pre-process SQL Statement.
			static bool parse(String &statement, const char *text, bool except = true);

			/// @brief Pre-process SQL Statement.
			/// @param node XML node with statement
			/// @param except 
			/// @return 
			static String parse(const Properties &props, bool except = true);

			static String parse(const Properties &props, const char *name, bool except = true);

			/// @brief Set SQL statements to execute.
			/// @param sql The SQL statements.
			void set(const char *text);

			/// @brief Execute SQL query, get response.
			void exec(const char *dbname, Udjat::Variant &values) const;

			void exec(const char *dbname, const Udjat::Variant &request, Udjat::Variant &response) const;

			static void exec(const char *dbname, const Properties &props, const char *name, bool required = false);

		};

	}
 }

