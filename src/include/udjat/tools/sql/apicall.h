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
  * @brief Implements SQL API Calls.
  */

/*
 #pragma once
 #include <udjat/defs.h>
 #include <udjat/worker.h>
 #include <udjat/tools/method.h>
 #include <udjat/tools/sql/script.h>
 #include <udjat/tools/abstract/request-path.h>

 namespace Udjat {

	namespace SQL {

		/// @brief Map an Udjat::Worker path to SQL Query.
		class UDJAT_API ApiCall : public RequestPath, private SQL::Script {
		private:
			Worker::ResponseType type = Worker::None;	///< @brief Response type for this query.

		public:
			ApiCall(const XML::Node &node);

			inline operator Worker::ResponseType() const noexcept {
				return type;
			}

			template <typename T>
			bool exec(Request &request, T &response) const {
				head(request,response);
				Script::exec(request,response);
				return true;
			}

		};


	}

 }
*/
