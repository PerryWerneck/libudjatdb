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
  * @brief Implements SQL factory.
  */

 #include <config.h>
 #include <config.h>
 #include <private/module.h>
 #include <udjat/factory.h>
 #include <udjat/tools/xml.h>
 #include <udjat/tools/value.h>
 #include <udjat/tools/sql/statement.h>
 #include <udjat/agent.h>
 #include <udjat/alert.h>
 #include <udjat/tools/quark.h>
 #include <udjat/agent/sql.h>

 using namespace std;

 namespace Udjat {

	std::shared_ptr<Abstract::Agent> SQL::Module::AgentFactory(const Abstract::Object &parent, const pugi::xml_node &node) const {

		if( !strcasecmp(node.name(),"sql") && strcasecmp(node.attribute("type").as_string(),"agent")) {
			return Udjat::Factory::AgentFactory(parent,node);
		}

		debug("--- Creating an SQL agent ---");

		switch(Udjat::XML::StringFactory(node,"value-type","string").select("integer","signed","unsigned","float","string",nullptr)) {
		case 0: // Integer
		case 1: // Signed
			return make_shared<SQL::Agent<int>>(node);

		case 2: // Unsigned
			return make_shared<SQL::Agent<unsigned int>>(node);

		case 3: // Float
			return make_shared<SQL::Agent<float>>(node);

		case 4: // String
			return make_shared<SQL::Agent<string>>(node);

		}

		// No type, create an string agent.
		Logger::String{"Unexpected value type, using 'string'"}.warning(Factory::name());
		return make_shared<SQL::Agent<string>>(node);

	}

	bool SQL::Module::generic(const pugi::xml_node &node) {

		switch(Udjat::XML::StringFactory(node,"type").select("initializer","url-scheme",nullptr)) {
		case 0: // Initializer
			debug("Initializer");
			SQL::Statement::exec(node);
			break;

		case 1: // URL Scheme
			debug("URL-Scheme");
			SQL::Statement{node};
			break;

		default:
			debug("Unknown");
			return false;
		}

		return true;

	}

 }


