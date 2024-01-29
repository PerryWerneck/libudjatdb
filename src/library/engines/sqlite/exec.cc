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

 /**
  * @brief Brief description of this source.
  */

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/sql/statement.h>
 #include <udjat/tools/value.h>
 #include <string>
 #include <sqlite3.h>
 #include <private/sqlite.h>

 using namespace std;

 namespace Udjat {

	/*
	void SQL::bind(const SQL::Statement::Script &script, cppdb::statement &stmt, const Abstract::Object &request, Udjat::Value &response) {

		for(const char *name : script.parameter_names) {

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

	}

	void SQL::parse_result(cppdb::result &res, Udjat::Value &response) {
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

	void SQL::exec(cppdb::session &session, const std::vector<SQL::Statement::Script> &scripts, const Abstract::Object &request, Udjat::Value &response) {

		debug(__FUNCTION__);

		for(auto &script : scripts) {
			if(script.text && *script.text) {
				auto stmt = session.create_statement(script.text);
				bind(script,stmt,request,response);

				if(strcasestr(script.text,"select")) {
					auto res = stmt.row();
					parse_result(res,response);
				} else {
					stmt.exec();
				}
			}
		}
	}
	*/

	void SQL::Statement::exec(const Udjat::Object &request) const {

		debug(__FUNCTION__);

		auto values = Udjat::Value::ObjectFactory();
		SQL::Session{dburl}.exec(scripts,request,*values);

	}

	void SQL::Statement::exec(std::shared_ptr<Udjat::Value> response) const {

		debug(__FUNCTION__);
		SQL::Session{dburl}.exec(scripts,*response);

	}

	void SQL::Statement::exec(const Udjat::Object &request, Udjat::Value &response) const {

		debug(__FUNCTION__);
		SQL::Session{dburl}.exec(scripts,request,response);

	}

	void SQL::Statement::exec(const Request &request, Udjat::Value &response) const {

		debug(__FUNCTION__,"::Value start");
		SQL::Session{dburl}.exec(scripts,request,response);
		debug(__FUNCTION__,"::Value ends");

	}

	void SQL::Statement::exec(const Request &request, Udjat::Response::Table &response) const {

		debug(__FUNCTION__,"::Table start");
		SQL::Session{dburl}.exec(scripts,request,response);
		debug(__FUNCTION__,"::Table ends");
	}

 }

