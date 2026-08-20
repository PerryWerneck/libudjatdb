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
 #include <udjat/tools/sql/script.h>
 #include <udjat/tools/properties.h>
 #include <udjat/tools/string.h>
 #include <udjat/tools/logger.h>
 #include <stdexcept>

 using namespace std;

 namespace Udjat {

	bool SQL::Script::parse(String &sql, const char *text, bool except) {

		sql.clear();

		String script{text};
		script.strip();
		if(script.empty()) {
			if(!except) {
				return false;
			}
			throw runtime_error("SQL Script is empty");
		}

		debug("-----> '",script.c_str(),"'");
		std::vector<String> lines = script.split("\n");
		debug("Number of lines: ",lines.size());
		debug("0='",lines[0].c_str(),"'");

		for(String &line : lines) {
			line.strip();
			debug("line='",line.c_str(),"'");
			if(line.empty()) {
				continue;
			}
			sql += line;
			if(line[line.size()-1] != ';') {
				sql += " ";
			}
		}

		debug(sql.c_str());
		sql.strip();
		debug(sql.c_str());

		{
			size_t length = sql.size();
			if(length > 1 && sql[length-1] == ';') {
				sql.resize(length-1);
				sql.strip();
			}
		}

		debug("SQL Query (size=",sql.size(),"):",sql.c_str());

		if(sql.empty()) {
			if(!except) {
				return false;
			}
			throw runtime_error("SQL Script is empty");
		}

		return true;
	}

	String SQL::Script::parse(const Properties &props, bool except) {
		String sql;
		parse(sql,props.child_value().c_str());
		return sql;
	}

	String SQL::Script::parse(const Properties &props, const char *name, bool except) {

		if(!props.has_child(name)) {
			if(except) {
				throw runtime_error(Logger::String{"Cant find required child '",name,"'"});
			} else {
				Logger::String{"Required child '",name,"' is not available"}.trace("sql");
			}
			return "";
		}

		String sql;

		props.for_each_child(name,[&sql](const Properties &child){
			parse(sql,child.child_value().c_str());
			return true;
		});
		
		return sql;

	}

	void SQL::Script::set(const char *text) {
		parse(sql,text);
	} 

	SQL::Script::Script(const Properties &props) {
		set(props.child_value().c_str());
	}

	SQL::Script::Script(const char *text) {
		set(text);
	}

	void SQL::Script::exec(const char *dbname, const Properties &props, const char *name, bool required) {

		String sql{SQL::Script::parse(props,name,required)};
		if(sql.empty()) {
			return;
		}

		Udjat::Variant value;
		Script{sql}.exec(dbname,value);

	}


 }
