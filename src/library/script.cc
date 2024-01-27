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


 #include <udjat/tools/sql/statement.h>

 using namespace std;

 namespace Udjat {

	SQL::Statement::Script::Script(const char *script) {

		if(!(script && *script)) {
			throw runtime_error("Rejecting build of an empty SQL script");
		}

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

	void SQL::Statement::Script::exec(Session &session, const Abstract::Object &object) const {

		auto stmt = session.create_statement(this->text);
		bind(stmt,object);

	}

	void SQL::Statement::Script::exec(cppdb::statement &stmt, Udjat::Value &response) const {

		debug("Running '",this->text,"'");

		if(strcasestr(this->text,"select")) {
			auto res = stmt.row();
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
		} else {
			stmt.exec();
		}

	}

	void SQL::Statement::Script::exec(Session &session, std::shared_ptr<Udjat::Value> response) const {

		if(!(this->text && *this->text)) {
			throw runtime_error("Cant execute an empty SQL script");
		}

		debug("Running '",this->text,"'");
		auto stmt = create_statement(session, *response);
		exec(stmt, *response);

	}

	cppdb::statement SQL::Statement::Script::create_statement(Session &session, const Abstract::Object &request, const Abstract::Object &response) const {

		if(!(this->text && *this->text)) {
			throw runtime_error("Cant execute an empty SQL script");
		}

		debug("Running '",this->text,"'");
		auto stmt = session.create_statement(this->text);

		for(const char *name : parameter_names) {

			string value;

			if(request.getProperty(name,value)) {

				debug("value(",name,")='",value,"' (from request)");
				stmt.bind(value);

			} else if(response.getProperty(name,value)) {

				debug("value(",name,")='",value,"' (from response)");
				stmt.bind(value);

			} else {

				throw runtime_error(Logger::String{"Required property '",name,"' is missing"});

			}

		}

		return stmt;

	}

	cppdb::statement SQL::Statement::Script::create_statement(Session &session, const Abstract::Object &object) const {

		if(!(this->text && *this->text)) {
			throw runtime_error("Cant execute an empty SQL script");
		}

		debug("Running '",this->text,"'");
		auto stmt = session.create_statement(this->text);

		for(auto name : parameter_names) {

			string value;

			if(object.getProperty(name,value)) {

				debug("value(",name,")='",value,"' (from object)");
				stmt.bind(value);

			} else {

				throw runtime_error(Logger::String{"Required property '",name,"' is missing"});

			}

		}

		return stmt;

	}

	void SQL::Statement::Script::exec(Session &session, const Abstract::Object &request, Udjat::Value &response) const {

		if(!(this->text && *this->text)) {
			throw runtime_error("Cant execute an empty SQL script");
		}

		auto stmt = create_statement(session,request,response);
		exec(stmt,response);

	}

 }
