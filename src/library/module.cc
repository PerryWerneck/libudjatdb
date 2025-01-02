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

 #include <config.h>
 #include <udjat/defs.h>

 #include <private/module.h>
 #include <private/urlqueue.h>
 #include <udjat/tools/sql/module.h>
 #include <udjat/tools/value.h>
 #include <udjat/agent/sql.h>
 #include <udjat/tools/sql/script.h>
 #include <udjat/agent/abstract.h>
 #include <udjat/tools/protocol.h>

 #include <memory>

 using namespace std;

 namespace Udjat {

	SQL::Module::Module(const char *name) 
		: Udjat::Module{name,SQL::module_info}, Abstract::Agent::Factory("sql") {
	}

	SQL::Module::~Module() {
	}

	std::shared_ptr<Abstract::Agent> SQL::Module::AgentFactory(const Abstract::Object &parent, const XML::Node &node) const {

		if(String{node,"url-queue-name"}.empty()) {
			//
			// Standard SQL Agent.
			//
			debug("---------------------> Building Agent '",node.attribute("name").as_string(),"'");
			switch(Value::TypeFactory(node,"value-type","int")) {
			case Value::String:
				return make_shared<SQL::Agent<string>>(node);

			case Value::Signed:
				return make_shared<SQL::Agent<int>>(node);

			case Value::Unsigned:
				return make_shared<SQL::Agent<unsigned int>>(node);

			case Value::Real:
			case Value::Fraction:
				return make_shared<SQL::Agent<double>>(node);

			case Value::Boolean:
				return make_shared<SQL::Agent<bool>>(node);

			default:
				throw logic_error("Invalid attribute: value-type");
			}
		}

		debug("---------------------> Building URL queue '",node.attribute("name").as_string(),"'");
		return make_shared<URLQueue>(node);

	}

 }

