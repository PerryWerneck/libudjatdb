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
  * @brief Implements SQL statement.
  */

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/version.h>
 #include <udjat/tools/xml.h>
 #include <udjat/tools/object.h>
 #include <cppdb/frontend.h>
 #include <vector>
 #include <stdexcept>
 #include <udjat/tools/string.h>
 #include <udjat/tools/logger.h>
 #include <udjat/tools/object.h>
 #include <udjat/tools/configuration.h>

 #include <udjat/sql/statement.h>
 #include <private/session.h>

 using namespace std;

 namespace Udjat {

	/// @brief Parse query string.
	static String get_sql_script(const XML::Node &node) {

		String query{node.child_value()};
		query.strip();

		// Remove line breaks.
		{
			debug("Query='",query.c_str(),"'");
			auto lines = query.split("\n");
			query.clear();
			for(auto &line : lines) {
				debug("line='",line.c_str(),"'");
				line.strip();
				if(!line.empty()) {
					if(!query.empty()) {
						query += " ";
					}
					query += line;
				}
			}

		}

		debug(query);

		return query;
	}

	SQL::Statement::Statement(const XML::Node &node) : dburl{Object::getAttribute(node,"database","connection","")} {

		if(!(dburl && *dburl)) {
			throw runtime_error("Required attribute 'connection' is missing or invalid");
		}

		// Parse query
		XML::Node script = node.child("script");
		if(script) {
			//

			// Scan for SQL properties.


			// Scan for SQL scripts
			while(script) {
				scripts.emplace_back(get_sql_script(script).c_str());
				script = script.next_sibling("script");
			}

		} else {

			scripts.emplace_back(get_sql_script(node).c_str());

		}

	}

	SQL::Statement::~Statement() {
	}

	void SQL::Statement::exec() {
	}

	void SQL::Statement::exec(const XML::Node &node) {

		String connection;

		// Get connection string
		{
			auto attribute = Object::getAttribute(node,"connection");
			if(attribute) {
				connection = attribute.as_string();
			} else {
				connection = Config::Value<string>("database","connection","").c_str();
			}
		}

		connection.expand(node).strip();
		if(connection.empty()) {
			throw runtime_error("Required attribute 'connection' is invalid or missing");
		}

		cppdb::session{connection.c_str()}.create_statement(get_sql_script(node)).exec();

	}

	void SQL::Statement::exec(const Udjat::Object &request) {

		cppdb::session session{dburl};

		cppdb::transaction guard(session);
		for(auto &script : scripts) {
			script.exec(session,request);
		}
		guard.commit();

	}

 }
