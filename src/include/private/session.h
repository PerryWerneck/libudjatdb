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
  * @brief Declares Session object.
  */

 #pragma once
 #include <udjat/defs.h>
 #include <udjat/tools/xml.h>
 #include <cppdb/frontend.h>
 #include <vector>
 #include <memory>
 #include <thread>

 namespace Udjat {

	namespace SQL {

		class UDJAT_PRIVATE Session : public cppdb::session {
		public:

			/// @brief Get default session.
			static std::shared_ptr<Session> getInstance();

			/// @brief Get session from XML node.
			static std::shared_ptr<Session> getInstance(const XML::Node &node);

		};

	}

 }
