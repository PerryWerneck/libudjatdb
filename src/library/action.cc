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
  * @brief Implements SQL based action.
  */

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/action.h>
 #include <udjat/tools/sql/script.h>
 #include <udjat/tools/sql/action.h>
 #include <memory>

 using namespace std;
 
 namespace Udjat {

	std::shared_ptr<Udjat::Action> SQL::Action::Factory::ActionFactory(const XML::Node &node) const {
		return make_shared<SQL::Action>(node);
	}

	SQL::Action::Action(const XML::Node &node) 
		: Udjat::Action{node}, SQL::Script{node}, dbname{String{node,"database-connection"}.as_quark()} {
		if(!(dbname && *dbname)) {
			throw runtime_error("Required attribute 'database-connection' is missing or empty");
		}
	}

	int SQL::Action::call(const Udjat::Value &request, Udjat::Value &response, bool except) {
		return Udjat::Action::exec(response,except,[&]() {
			SQL::Script::exec(dbname,request,response);
			return 0;
		});
	}

 }