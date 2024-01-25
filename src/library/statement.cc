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
 #include <udjat/tools/sql/statement.h>
 #include <private/session.h>
 #include <udjat/tools/abstract/response.h>

 using namespace std;

 namespace Udjat {

 	static String connection_from_xml(const XML::Node &node) {

		for(XML::Node parent = node; parent; parent = parent.parent())  {

			XML::Attribute attr;

			attr = parent.attribute("connection");
			if(attr) {
				return String{attr.as_string()}.expand(node).strip();
			}

			attr = parent.attribute("database-connection");
			if(attr) {
				return String{attr.as_string()}.expand().strip();
			}

			for(XML::Node child = parent.child("attribute");child;child = child.next_sibling("attribute")) {

				if(!strcasecmp(child.attribute("name").as_string(),"database-connection")) {
					return String{child.attribute("value").as_string()}.expand(node).strip();
				}

				if(!strcasecmp(child.attribute("name").as_string(),"database")) {
					return String{child.attribute("value").as_string()}.expand(node).strip();
				}

			}

 		}

 		Config::Value<string> connection{"database","connection",""};

 		if(!connection.empty()) {
			return connection.c_str();
 		}

 		throw runtime_error("Required attribute 'database-connection' is missing");

 	}

	SQL::Statement::Statement(const XML::Node &node, const char *child_name, bool allow_empty, bool allow_text)
		: dburl{connection_from_xml(node).as_quark()} {

		if(!(dburl && *dburl)) {
			throw runtime_error("Invalida database connection string");
		}

		// Parse query
		XML::Node script = node.child(child_name);

		if(script) {

			// Scan for SQL scripts
			debug("Parsing node <",script.name(),">");

			while(script) {
				push_back(script,allow_empty);
				script = script.next_sibling(child_name);
			}

		} else if(allow_text) {

			debug("Using node '",node.name(),"' as script");
			push_back(node,allow_empty);

		} else if(!allow_empty) {

			throw runtime_error(Logger::String{"Required child <",child_name,"> is missing or invalid"});

		}

	}

	const char * SQL::Statement::parse(Udjat::String &query) {

		query.strip();
		if(query.empty()) {
			return "";
		}

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
		return query.c_str();

	}

	void SQL::Statement::push_back(const XML::Node &node, bool allow_empty) {

		size_t lines = 0;
		for(auto &line : String{node.child_value()}.strip().split(";")) {
			line.strip();
			if(line.empty()) {
				continue;
			}

			parse(line);
			debug("Adding script '",line,"'");
			scripts.push_back(line.c_str());
			lines++;

		}

		if(lines == 0 && !allow_empty) {
			throw runtime_error(Logger::String{"Missing required contents on <",node.name(),"> node"});
		}

	}

	SQL::Statement::~Statement() {
	}

	void SQL::Statement::exec() const {
	}

	void SQL::Statement::exec(const XML::Node &node) {

		String connection{connection_from_xml(node)};
		if(connection.empty()) {
			throw runtime_error("Required attribute 'database-connection' is invalid or missing");
		}

		String script{node.child_value()};
		cppdb::session{connection.c_str()}.create_statement(parse(script)).exec();

	}

	void SQL::Statement::exec(const Udjat::Object &request) const {

		cppdb::session session{dburl};
		cppdb::transaction guard(session);

		for(auto &script : scripts) {
			script.exec(session,request);
		}

		guard.commit();

	}

	void SQL::Statement::exec(const Udjat::Object &request, Udjat::Value &response) const {

		cppdb::session session{dburl};
		cppdb::transaction guard(session);

		for(auto &script : scripts) {
			script.exec(session,request,response);
		}

		guard.commit();

	}

	void SQL::Statement::exec(const Request &request, Udjat::Value &response) const {

		debug(__FUNCTION__,"::Value start");

		cppdb::session session{dburl};
		cppdb::transaction guard(session);

		for(auto &script : scripts) {

			auto result = script.create_statement(session,request,response).row();
			debug("-----------------------> '",script.text,"'");
			if(!result.empty() && strncasecmp(script.text,"select",6) == 0) {
				// Transfer result to response.
				for(int col = 0; col < result.cols();col++) {
					string val;
					result.fetch(col,val);
					debug("----->",result.name(col).c_str(),"='",val.c_str(),"'");
					response[result.name(col).c_str()] = val.c_str();
				}
			}
		}

		debug(__FUNCTION__,"::Value end");
		guard.commit();
	}

	void SQL::Statement::exec(const Request &request, Udjat::Response::Table &response) const {

		debug(__FUNCTION__,"::Table start");

		cppdb::session session{dburl};
		cppdb::transaction guard(session);

		auto result = scripts[0].create_statement(session, request).row();
		if(!result.empty()) {

			// Get column names.
			std::vector<string> colnames;

			{
				// Get first line and column names.
				std::vector<string> values;
				for(int col = 0; col < result.cols();col++) {
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

			}

			size_t rows = 1;

			// Get other lines.

		} else {

			debug("Empty response");
			response.count(0);

		}

		guard.commit();

		debug(__FUNCTION__,"::Table end");
	}

 }
