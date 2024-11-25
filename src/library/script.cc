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
 #include <udjat/tools/string.h>
 #include <udjat/tools/sql/script.h>
 #include <udjat/tools/xml.h>

 using namespace std;

 namespace Udjat {

	void SQL::Script::set(const char *text) {

		sql.clear();

		for(String &line : String{text}.split("\n")) {
			line.strip();
			if(line.empty()) {
				continue;
			}
			sql += line;
			if(line[line.size()-1] != ';') {
				sql += " ";
			}
		}
		sql.strip();
		if(sql[sql.size()-1] == ';') {
			sql.resize(sql.size()-1);
		}
		debug("SQL Query:",sql.c_str());

	} 

	SQL::Script::Script(const XML::Node &node) {
		set(node.child_value());
	}

	SQL::Script::Script(const char *text) {
		set(text);
	}

 }


/*
 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/xml.h>
 #include <udjat/tools/object.h>
 #include <vector>
 #include <stdexcept>
 #include <udjat/tools/string.h>
 #include <udjat/tools/logger.h>
 #include <udjat/tools/object.h>
 #include <udjat/tools/quark.h>


 #include <udjat/tools/sql/script.h>

 using namespace std;

 namespace Udjat {

	SQL::Statement::Statement(const char *script) {

		if(!(script && *script)) {
			throw runtime_error("Rejecting build of an empty SQL script");
		}

		String text{script};
		size_t from = text.find("${");
		while(from != string::npos) {

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

 }
*/
