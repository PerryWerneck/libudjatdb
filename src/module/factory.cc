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
 #include <udjat/sql/statement.h>
 #include <udjat/agent.h>
 #include <udjat/tools/quark.h>

 using namespace std;

 namespace Udjat {

 	namespace SQL {

		template <typename T>
		class UDJAT_PRIVATE Agent : public Udjat::Agent<T> {
		private:

			/// @brief Query to update agent value.
			const SQL::Statement update;

			/// @brief The name of agent value got by SQL query.
			const char *value_name;

		public:

			Agent(const pugi::xml_node &node) :
				Udjat::Agent<T>{node}, update{node,"refresh"}, value_name{Quark{node,"value-from","value"}.c_str()} {
			}

			bool refresh(bool) override {

				if(!update.size()) {
					return false;
				}

				std::shared_ptr<Udjat::Value> value = Udjat::Value::ObjectFactory();
				update.exec(*this,*value);
				return this->assign((*value)[value_name].as_string().c_str());

			}

		};

 	}

	std::shared_ptr<Abstract::Agent> SQL::Module::AgentFactory(const Abstract::Object &parent, const pugi::xml_node &node) const {

		debug("-------------------");

		if( !strcasecmp(node.name(),"sql") && strcasecmp(node.attribute("type").as_string(),"agent")) {
			return Udjat::Factory::AgentFactory(parent,node);
		}

		debug("--- Create an SQL agent ---");


		// No type, create an string agent.
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


