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
  * @brief Declare SQL module.
  */

 #pragma once

 #include <udjat/defs.h>
 #include <udjat/module/abstract.h>
 #include <udjat/tools/action.h>
 #include <udjat/tools/sql/action.h>

 namespace Udjat {

	namespace SQL {

		class UDJAT_API Module : public Udjat::Module, private SQL::Action::Factory {
		public:

			Module(const char *name);
			virtual ~Module();

		};
	}

 }

/*
 #pragma once

 #include <udjat/module/info.h>
 #include <udjat/tools/worker.h>
 #include <udjat/factory.h>
 #include <udjat/tools/sql/apicall.h>
 
 namespace Udjat {

	namespace SQL {

		class UDJAT_API Module : public Udjat::Module, protected Udjat::Worker, protected Udjat::Factory {
		private:
			std::vector<SQL::ApiCall> queries;

		public:
			static Module * Factory(const char *name, const ModuleInfo &info);

			Module(const char *name, const ModuleInfo &info);
			virtual ~Module();

			Worker::ResponseType probe(const Request &request) const noexcept override;
			bool work(Request &request, Response::Table &response) const override;
			bool work(Request &request, Response::Value &response) const override;
			bool CustomFactory(Abstract::Object &, const XML::Node &node) override;
			bool generic(const pugi::xml_node &node) override;
			std::shared_ptr<Abstract::Agent> AgentFactory(const Abstract::Object &, const XML::Node &node) const override;
			std::shared_ptr<Abstract::Alert> AlertFactory(const Abstract::Object &, const XML::Node &node) const override;
			void trace_paths(const char *url_prefix) const noexcept override;

		};

	}

 }


*/
