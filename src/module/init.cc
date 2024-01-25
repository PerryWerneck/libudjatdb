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

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/module/abstract.h>
 #include <udjat/tools/worker.h>
 #include <udjat/module/abstract.h>
 #include <udjat/factory.h>
 #include <stdexcept>
 #include <udjat/tools/sql/statement.h>
 #include <udjat/tools/sql/query.h>
 #include <udjat/agent/sql.h>
 #include <udjat/tools/method.h>

 using namespace Udjat;
 using namespace std;

 /// @brief Register udjat module.
 Udjat::Module * udjat_module_init() {

	static const ModuleInfo module_info{"cppdb", "CPPDB SQL Module"};

	class Module : public Udjat::Module, private Udjat::Worker, private Udjat::Factory {
	private:

		std::vector<SQL::Query> queries;

	public:
		Module() : Udjat::Module("cppdb",module_info), Udjat::Worker("cppdb",module_info), Udjat::Factory("sql",module_info) {
		};

		~Module() {
		}

		void trace_paths(const char *url_prefix) const noexcept override {
			for(const auto &query : queries) {
				Logger::String{"SQL ",std::to_string((HTTP::Method) query)," available on ",url_prefix,query.c_str()}.trace("cppdb");
			}
		}

		// Udjat::Worker
		ResponseType probe(const Request &request) const noexcept override {

			for(const auto &query : queries) {
				if( (query == ((HTTP::Method) request) && (request == query.c_str()))) {
					debug("Accepting '",query.c_str(),"' as ",((Worker::ResponseType) query));
					return (Worker::ResponseType) query;
				}
			}

			debug("Rejecting '",request.path(),"'");
			return Worker::None;
		}

		bool work(Request &request, Response::Table &response) const override {

			for(const auto &query : queries) {
				if(query == ((HTTP::Method) request) && request.pop(query.c_str())) {
					debug(__FUNCTION__,"('",request.path(),"')");
					return query.exec(request,response);
				}
			}

			return false;

		}

		bool work(Request &request, Response::Value &response) const override {

			for(const auto &query : queries) {
				if(query == ((HTTP::Method) request) && request.pop(query.c_str())) {
					debug(__FUNCTION__,"('",request.path(),"')");
					return query.exec(request,response);
				}
			}

			return false;

		}

		// Udjat::Factory
		bool generic(const pugi::xml_node &node) override {

			switch(String{node,"type"}.select("initializer","url-scheme","query","api-call",nullptr)) {
			case 0: // Initializer
				debug("Initializer");
				SQL::Statement::exec(node);
				break;

			case 1: // URL Scheme
				debug("URL-Scheme");
				// SQL::Statement{node};
				break;

			case 2: // Query
			case 3: // api-call
				debug("API-Call/Query");
				queries.emplace_back(node);
				break;

			default:
				debug("Unknown");
				return false;
			}

			return true;

		}

		std::shared_ptr<Abstract::Agent> AgentFactory(const Abstract::Object &parent, const pugi::xml_node &node) const {

			if( !strcasecmp(node.name(),"sql") && strcasecmp(node.attribute("type").as_string(),"agent")) {
				return Udjat::Factory::AgentFactory(parent,node);
			}

			debug("--- Creating an SQL agent ---");
			std::shared_ptr<Abstract::Agent> agent;

			switch(String{node,"value-type","string"}.select("integer","signed","unsigned","float","string",nullptr)) {
			case 0: // Integer
			case 1: // Signed
				agent = make_shared<SQL::Agent<int>>(node);
				break;

			case 2: // Unsigned
				agent = make_shared<SQL::Agent<unsigned int>>(node);
				break;

			case 3: // Float
				agent = make_shared<SQL::Agent<float>>(node);
				break;

			case 4: // String
				agent = make_shared<SQL::Agent<string>>(node);
				break;

			default:
				// No type, create an string agent.
				Logger::String{"Unexpected value type, using 'string'"}.warning(Factory::name());
				agent = make_shared<SQL::Agent<string>>(node);
			}


			return agent;
		}

	};

	return new Module();

 }
