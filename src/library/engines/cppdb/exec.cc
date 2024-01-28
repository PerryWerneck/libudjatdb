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
 #include <cppdb/frontend.h>
 #include <private/cppdb.h>
 #include <string>

 using namespace std;

 namespace Udjat {

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

	void SQL::Statement::exec(const Udjat::Object &request) const {

		auto values = Udjat::Value::ObjectFactory();

		cppdb::session session{dburl};
		cppdb::transaction guard(session);

		SQL::exec(session,scripts,request,*values);

		guard.commit();

	}

	void SQL::Statement::exec(std::shared_ptr<Udjat::Value> response) const {

		cppdb::session session{dburl};
		cppdb::transaction guard(session);

		for(auto &script : scripts) {
			if(script.text && *script.text) {
				auto stmt = session.create_statement(script.text);
				if(strcasestr(script.text,"select")) {
					auto res = stmt.row();
					parse_result(res,*response);
				} else {
					stmt.exec();
				}
			}
		}

		guard.commit();

	}

	void SQL::Statement::exec(const Udjat::Object &request, Udjat::Value &response) const {

		cppdb::session session{dburl};
		cppdb::transaction guard(session);

		SQL::exec(session,scripts,request,response);

		guard.commit();

	}

	void SQL::Statement::exec(const Request &request, Udjat::Value &response) const {

		debug(__FUNCTION__,"::Value start");

		cppdb::session session{dburl};
		cppdb::transaction guard(session);

		SQL::exec(session,scripts,request,response);

		guard.commit();

		debug(__FUNCTION__,"::Value ends");

	}

	void SQL::Statement::exec(const Request &request, Udjat::Response::Table &response) const {

		debug(__FUNCTION__,"::Table start");

		cppdb::session session{dburl};
		cppdb::transaction guard(session);

		for(const auto &script : scripts) {

			if(strcasestr(script.text,"select")) {

				debug(__FUNCTION__,"('",script.text,"')");

				// It's a select, get report
				auto stmt = session.create_statement(script.text);
				for(const char *name : scripts[0].parameter_names) {

					string value;
					if(request.getProperty(name,value)) {

						debug("value(",name,")='",value,"' (from request)");
						stmt.bind(value);

					} else {

						throw runtime_error(Logger::String{"Required property '",name,"' is missing"});

					}

				}

				auto result = stmt.row();

				if(!result.empty()) {

					// Get first line and column names.
					int numcols = result.cols();
					std::vector<string> values;
					std::vector<string> colnames;

					for(int col = 0; col < numcols;col++) {
						string val;
						result.fetch(col,val);
						values.push_back(val);
						colnames.push_back(result.name(col));
					}

					// Start report...
					response.start(colnames);

					// ...and store first line
					for(auto &value : values) {
						response << value;
					}

					size_t rows = 1;

					// Get other lines.
					while(result.next()) {
						rows++;
						for(int col = 0; col < numcols;col++) {
							string value;
							result.fetch(col,value);
							response << value;
						}

					}
					response.count(rows);

				} else {
					debug("Empty response");
					response.count(0);
				}

			}
#ifdef DEBUG
			else {
				debug("Rejecting '",script.text,"'");
			}
#endif // DEBUG

		}

		guard.commit();

		debug(__FUNCTION__,"::Value ends");
	}

 }

