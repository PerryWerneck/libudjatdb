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

/*
 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/module/abstract.h>
 #include <udjat/tools/worker.h>
 #include <udjat/module/abstract.h>
 #include <udjat/tools/factory.h>
 #include <stdexcept>
 #include <udjat/tools/sql/script.h>
 #include <udjat/tools/sql/apicall.h>
 #include <udjat/agent/sql.h>
 #include <udjat/tools/method.h>
 #include <udjat/alert/sql.h>
 #include <private/urlqueue.h>
 #include <private/module.h>
 #include <udjat/agent/sql.h>
 #include <memory>

 #include <udjat/tools/sql/module.h>

 using namespace std;

 namespace Udjat {

	SQL::Module * SQL::Module::Factory(const char *name, const ModuleInfo &info) {
		return new SQL::Module(name,info);
	}

	SQL::Module::Module(const char *name, const ModuleInfo &info) 
		: Udjat::Module{name,info}, Udjat::Worker{name,info}, Udjat::Factory{name,info} {
	}

	SQL::Module::~Module() {
	}

	void SQL::Module::trace_paths(const char *url_prefix) const noexcept {
		for(const auto &query : queries) {
			Logger::String{"SQL ",std::to_string((HTTP::Method) query)," available on ",url_prefix,query.path()}.trace("cppdb");
		}
	}

	// Udjat::Worker
	Worker::ResponseType SQL::Module::probe(const Request &request) const noexcept {

		for(const auto &query : queries) {
			debug("query='",query.path(),"' request='",request.path(),"'");
			if(query == request) {
				debug("Accepting '",query.path(),"' as ",((Worker::ResponseType) query));
				return (Worker::ResponseType) query;
			}
		}

		debug("Rejecting '",request.path(),"'");
		return Worker::None;
	}

	bool SQL::Module::work(Request &request, Response::Table &response) const {

		for(const auto &query : queries) {
			if(query == request && request.pop(query.path())) {
				debug(__FUNCTION__,"('",request.path(),"')");
				return query.exec(request,response);
			}
		}

		return false;

	}

	bool SQL::Module::work(Request &request, Response::Value &response) const {

		for(const auto &query : queries) {
			if(query == request && request.pop(query.path())) {
				debug(__FUNCTION__,"('",request.path(),"')");
				return query.exec(request,response);
			}
		}

		return false;

	}

	// Udjat::Factory
	bool SQL::Module::CustomFactory(Abstract::Object &, const XML::Node &node) {
		switch(String{node.name()}.select("init","url-scheme","api-call",nullptr)) {
		case 0: // Init
			debug("Init script");
			SQL::Script::exec(node);
			break;

		case 1: // URL Scheme
			debug("URL-Scheme");
			break;

		case 2: // api-call
			debug("API-Call");
			queries.emplace_back(node);
			break;

		default:
			debug("Unknown6");
			return false;
		}
		return true;
	}

	bool SQL::Module::generic(const pugi::xml_node &node) {

		switch(String{node,"type"}.select("initializer","url-scheme","query","api-call",nullptr)) {
		case 0: // Initializer
			debug("Initializer");
			SQL::Script::exec(node);
			break;

		case 1: // URL Scheme
			debug("URL-Scheme");
			// SQL::Script{node};
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

	std::shared_ptr<Abstract::Agent> SQL::Module::AgentFactory(const Abstract::Object &, const XML::Node &node) const {

		debug("--- Creating an SQL agent ---");

		if(node.attribute("url-queue-name")) {
			return make_shared<SQL::URLQueue>(node);
		}

		//
		// Try standard agents.
		//
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

	std::shared_ptr<Abstract::Alert> SQL::Module::AlertFactory(const Abstract::Object &, const XML::Node &node) const {
		return make_shared<SQL::Alert>(node);
	}

}

*/
