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
 #include <udjat/tools/request.h>
 #include <udjat/tools/response.h>
 #include <udjat/tools/report.h>
 #include <vector>
 #include <memory>

 namespace Udjat {

	namespace SQL {

		/// @brief Get engine name
		/// @return The engine name ("sqlite", "cppdb", ...)
		UDJAT_API const char * engine() noexcept;

		/// @brief A single SQL statement.
		class UDJAT_API Statement {
		public:
			const char *text;
			std::vector<const char *> parameter_names;
			Statement(const char *script);

		};

		/// @brief An SQL statement.
		class UDJAT_API Script {
		public:

			/// @brief Create SQL statement from XML definition.
			/// @param node the parent node.
			/// @param child_name The XML tagname for the script nodes.
			/// @param allow_empty Allow empty scripts.
			/// @param allo_text Allow using node 'cdata' for script text.
			Script(const XML::Node &node, const char *child_name = "script", bool allow_empty = false, bool allow_text = true);

			virtual ~Script();

			/// @brief False if query is empty.
			inline size_t size() const noexcept {
				return scripts.size();
			}

			/// @brief Execute query without any parameters.
			void exec() const;

			/// @brief Execute SQL query
			/// @param request The object with the values.
			void exec(const Udjat::Object &request) const;

			/// @brief Execute SQL query, get response.
			void exec(const Udjat::Object &request, Udjat::Value &response) const;

			/// @brief Execute SQL query, get response.
			void exec(const Udjat::Object &request, Udjat::Response::Table &response) const;

			/// @brief Execute SQL query, get response.
			void exec(const Request &request, Udjat::Value &response) const;

			/// @brief Execute SQL query, get response.
			void exec(const Request &request, Udjat::Response::Table &response) const;

			void exec(std::shared_ptr<Udjat::Value> response) const;

			/// @brief Execute SQL query.
			static void exec(const XML::Node &node);

			/// @brief Execute <init> children.
			static void init(const XML::Node &node);


		private:

			/// @brief The database URL;
			const char *dburl = nullptr;

			std::vector<Statement> scripts;

			static const char * parse(Udjat::String &query);
			void push_back(const XML::Node &node, bool allow_empty = false);

		public:

			inline const char *dbconn() const noexcept {
				return dburl;
			}

			inline const auto begin() const {
				return scripts.begin();
			}

			inline const auto end() const {
				return scripts.end();
			}

		};

	}

 }

