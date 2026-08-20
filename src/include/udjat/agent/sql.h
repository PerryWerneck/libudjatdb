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
 #include <udjat/tools/properties.h>
 #include <udjat/agent.h>
 #include <udjat/tools/sql/script.h>
 #include <udjat/tools/variant.h>

 namespace Udjat {

	namespace SQL {

		template <typename T>
		class UDJAT_API Agent : public Udjat::Agent<T> {
		protected:

			/// @brief The name of agent value got by SQL query.
			const char *valuename;

			/// @brief URL for database connection.
			const char *dbname;

			/// @brief SQL Script to update agent value.
			String update;

		public:

			Agent(const Properties &props) :
				Udjat::Agent<T>{props},
				valuename{props.get("value-from","value").as_quark()},
				dbname{props.get("database-connection").as_quark()},
				update{SQL::Script::parse(props,"refresh")} {
				SQL::Script::exec(dbname,props,"init");
			}

			bool refresh(bool b) override {
				Value values;
				T val = this->get();
				values[valuename] = val;
				SQL::Script{update}.exec(dbname,values);
				values[valuename].get(val);
				return this->set(val);
			}

		};

	}

 }
