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
  * @brief Brief description of this source.
  */

 #pragma once

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/sql/statement.h>
 #include <udjat/tools/value.h>
 #include <udjat/tools/abstract/object.h>
 #include <cppdb/frontend.h>

 namespace Udjat {

	namespace SQL {

		void bind(const SQL::Statement::Script &script, cppdb::statement &stmt, const Abstract::Object &request, Udjat::Value &response);
		void exec(cppdb::session &session, const std::vector<SQL::Statement::Script> &scripts, const Abstract::Object &request, Udjat::Value &response);
		void parse_result(cppdb::result &res, Udjat::Value &response);

	}

 }
