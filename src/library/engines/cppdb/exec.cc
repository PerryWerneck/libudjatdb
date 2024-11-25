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
 #include <udjat/tools/sql/script.h>
 #include <udjat/tools/value.h>
 #include <udjat/tools/report.h>
 #include <udjat/tools/timestamp.h>
 #include <cppdb/frontend.h>
 #include <private/cppdb.h>
 #include <string>
 #include <stdexcept>

 using namespace std;

 namespace Udjat {

	static void bind(cppdb::statement &stmt, int col, const Udjat::Value &value) {

		switch((Value::Type) value) {
		case Value::Undefined:
			stmt.bind_null(col);
			break;

		case Value::String:
		case Value::Icon:
		case Value::Url:
			stmt.bind(col,value.c_str());
			break;
			
		case Value::Timestamp:
			{				
				TimeStamp time;
				value.get(time);
				stmt.bind(col,(struct tm) time);
			}
			break;
			
		case Value::Signed:
			{
				int v;
				value.get(v);
				stmt.bind(col,v);
			}
			break;
			
		case Value::Unsigned:
		case Value::Boolean:
		case Value::State:
			{
				unsigned int v;
				value.get(v);
				stmt.bind(col,v);
			}
			break;
			
		case Value::Real:
		case Value::Fraction:
			{
				double v;
				value.get(v);
				stmt.bind(col,v);
			}
			break;
			
		default:
			throw runtime_error("Unsupported value type");			

		}

	}

	void SQL::Script::exec(const char *dbname, Udjat::Value &values) const {
		exec(dbname,values,values);
	}

	void SQL::Script::exec(const char *dbname, const Udjat::Value &request, Udjat::Value &response) const {
		Session{dbname}.exec(sql,request,response);
	}

	void SQL::Session::exec(Udjat::String statement, const Udjat::Value &request, Udjat::Value &response, const char *name) {

		cppdb::transaction guard(*this);

		for(String &line : statement.split(";")) {

			if(Logger::enabled(Logger::Trace)) {
				Logger::String{line.c_str()}.trace("sql");
			}

			// Prepare SQL statement.
			vector<string> column_names;

			line.expand([&](const char *key, std::string &value){
				column_names.push_back(key);
				value = "?";
				return true;
			},false,true);

			debug("SQL Processed: ",line.c_str());

			auto stmt = create_statement(line);

			// Bind values.
			int column = 1;
			for(const auto &cname: column_names) {
				if(request.contains(cname.c_str())) {
					bind(stmt,column,request[cname.c_str()]);
				} else if(response.contains(cname.c_str())) {
					bind(stmt,column,response[cname.c_str()]);
				} else {
					throw logic_error(Logger::String{"Required column '",cname.c_str(),"' not found"});
				}
				column++;
			}

			try {

				cppdb::result result = stmt.row();
				if(result.empty()) {
					debug("Empty result");
					continue;
				}

				Udjat::Value row;
				get(result,row);

				if(result.next()) {

					// Has a second line, convert to report.
					Value &repoval = response;
					if(name) {
						repoval = response[name];
					} else {
						repoval.clear();
					}

					std::vector<string> names;

					for(int col = 0; col < result.cols();col++) {
						names.push_back(result.name(col));
					}

					auto &report = repoval.ReportFactory(names);
					report.push_back(row);

					do {
						get(result,report);
					} while(result.next());

				} else {

					// No second line, merge row into response.
					response.merge(row);


				}

			} catch(const std::exception &e) {

				Logger::String{e.what()}.trace("cppdb");

			}


		}

		guard.commit();

	}

 }
	/*
	void SQL::bind(const SQL::Statement &script, cppdb::statement &stmt, const Abstract::Object &request, Udjat::Value &response) {

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

	void SQL::exec(cppdb::session &session, const std::vector<SQL::Statement> &scripts, const Abstract::Object &request, Udjat::Value &response) {

		debug(__FUNCTION__);

		for(auto &script : scripts) {
			if(script.text && *script.text) {
				if(Logger::enabled(Logger::Trace)) {
					Logger::String{script.text}.trace("sql");
				}
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

	void SQL::Script::exec(const Udjat::Object &request) const {

		debug(__FUNCTION__);

		auto values = Udjat::Value::ObjectFactory();

		cppdb::session session{dburl};
		cppdb::transaction guard(session);

		SQL::exec(session,scripts,request,*values);

		guard.commit();

	}

	void SQL::Script::exec(std::shared_ptr<Udjat::Value> response) const {

		debug(__FUNCTION__);

		cppdb::session session{dburl};
		cppdb::transaction guard(session);

		for(auto &script : scripts) {

			if(script.text && *script.text) {
				auto stmt = session.create_statement(script.text);

				for(const char *name : script.parameter_names) {

					string value;

					if(response->getProperty(name,value)) {

						debug("value(",name,")='",value,"' (from response)");
						stmt.bind(value);

					} else {

						throw runtime_error(Logger::String{"Required property '",name,"' is missing"});

					}
				}

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

	void SQL::Script::exec(const Udjat::Object &request, Udjat::Value &response) const {

		debug(__FUNCTION__);

		cppdb::session session{dburl};
		cppdb::transaction guard(session);

		SQL::exec(session,scripts,request,response);

		guard.commit();

	}

	void SQL::Script::exec(const Request &request, Udjat::Value &response) const {

		debug(__FUNCTION__,"::Value start");

		cppdb::session session{dburl};
		cppdb::transaction guard(session);

		SQL::exec(session,scripts,request,response);

		guard.commit();

		debug(__FUNCTION__,"::Value ends");

	}

	void SQL::Script::exec(const Request &request, Udjat::Response::Table &response) const {

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

	*/
