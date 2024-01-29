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
  * @brief Declare SQL Agent for udjat.
  */

 #pragma once
 #include <udjat/defs.h>
 #include <udjat/tools/xml.h>
 #include <udjat/agent/abstract.h>
 #include <udjat/tools/sql/script.h>
 #include <udjat/agent.h>

 namespace Udjat {

	namespace SQL {

		template <typename T>
		class UDJAT_API Agent : public Udjat::Agent<T> {
		private:

			/// @brief SQL Script to update agent value.
			const SQL::Statement update;

			/// @brief SQL Script to get properties
			const SQL::Statement properties;

			/// @brief The name of agent value got by SQL query.
			const char *value_name;

		public:

			Agent(const XML::Node &node) :
				Udjat::Agent<T>{node},
					update{node,"refresh",true,false},
					properties{node,"properties",true,false},
					value_name{Quark{node,"value-from","value"}.c_str()} {
				SQL::Statement::init(node);
			}

			bool refresh(bool) override {

				if(!update.size()) {
					return false;
				}

				std::shared_ptr<Udjat::Value> value = Udjat::Value::ObjectFactory();
				update.exec(*this,*value);
				return this->assign((*value)[value_name].as_string().c_str());

			}

			bool getProperties(const char *path, Value &value) const override {

				if(properties.size()) {
					properties.exec(*this,value);
					return true;
				}

				return Udjat::Agent<T>::getProperties(path,value);
			}

		};

	}


 }

