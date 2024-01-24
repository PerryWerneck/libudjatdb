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
  * @brief Brief description of this source.
  */

 #pragma once
 #include <udjat/defs.h>
 #include <udjat/worker.h>
 #include <udjat/tools/method.h>
 #include <udjat/tools/sql/statement.h>

 namespace Udjat {

	namespace SQL {

		/// @brief Map an Udjat::Worker path to SQL Query.
		class UDJAT_PRIVATE Query : private SQL::Statement {
		private:
			const char *path;								///< @brief Path for URL request.
			Worker::ResponseType type = Worker::None;	///< @brief Response type for this query.
			HTTP::Method method = HTTP::Get;
			time_t expires = 300;

			void head(const Request &request, Abstract::Response &response) const;

		public:
			Query(const XML::Node &node);

			inline operator Worker::ResponseType() const noexcept {
				return type;
			}

			inline operator HTTP::Method() const noexcept {
				return method;
			}

			inline const char * c_str() const noexcept {
				return path;
			}

			bool operator==(const char *p) const noexcept;

			inline bool operator==(HTTP::Method m) const noexcept {
				return m == method;
			}

			bool exec(Request &request, Response::Value &response) const;

			bool exec(Request &request, Response::Table &response) const;

		};


	}

 }
