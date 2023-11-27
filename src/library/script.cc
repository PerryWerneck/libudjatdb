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
  * @brief Implements SQL script.
  */

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/xml.h>
 #include <udjat/tools/object.h>
 #include <cppdb/frontend.h>
 #include <vector>
 #include <stdexcept>
 #include <udjat/tools/string.h>
 #include <udjat/tools/logger.h>
 #include <udjat/tools/object.h>
 #include <udjat/tools/quark.h>


 #include <udjat/sql/statement.h>

 using namespace std;

 namespace Udjat {

	SQL::Statement::Script::Script(const char *script) {

		String text{script};
		size_t from = text.find("${");
		while(from != string::npos) {

			cout << from << endl;

			size_t to = text.find("}",from+2);
			if(to == string::npos) {
				throw runtime_error("Invalid parameter formatting");
			}

			parameter_names.emplace_back(Quark{text.substr(from+2,(to-(from+2)))}.c_str());
			text.std::string::replace(from,(size_t) (to-from)+1, "?");
			from = text.find("${",from);

		}

		this->text = text.strip().as_quark();

	}

	void SQL::Statement::Script::exec(Session &session, const Udjat::Object &object) {

		auto stmt = session.create_statement(this->text);

		for(auto name : parameter_names) {
			string value;
			if(!object.getProperty(name,value)) {
				throw runtime_error(Logger::Message{"Required property '",name,"' is missing"});
			}
			debug("value='",value,"'");
			stmt.bind(value);
		}

		stmt.exec();

	}

	void SQL::Statement::Script::exec(Session &session, const Udjat::Object &request, Udjat::Value &response) {

		auto stmt = session.create_statement(this->text);

		for(auto name : parameter_names) {
			string value;
			Udjat::Value &prop = response[name];

			if(!prop.isNull()) {	// First check if the property is in the response.
				value = prop.as_string();
			} else if(!request.getProperty(name,value)) {	// Not in response, try source object.
				throw runtime_error(Logger::Message{"Required property '",name,"' is missing"});
			}

			debug("value='",value,"'");
			stmt.bind(value);

		}

		cppdb::result res = stmt.row();

		if(!res.empty()) {

			// Got result update response;
			debug("Got response from SQL query");

			for(int col = 0; col < res.cols();col++) {
				string val;
				res.fetch(col,val);
				debug(res.name(col).c_str(),"='",val.c_str(),"'");
				response[res.name(col).c_str()] = val.c_str();
			}

		}


	}

 }
