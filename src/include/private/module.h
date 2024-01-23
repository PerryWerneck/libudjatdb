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
  * @brief Declare module clas.
  */

 #pragma once

 #include <udjat/defs.h>
 #include <udjat/module.h>
 #include <udjat/worker.h>
 #include <udjat/factory.h>
 #include <private/controller.h>
 #include <vector>

 namespace Udjat {

	namespace SQL {

		class UDJAT_PRIVATE Module : public Udjat::Module, private Udjat::Worker, private SQL::Controller, private Udjat::Factory {
		private:

			/// @brief Map an Udjat::Worker path to SQL Query.
			class UDJAT_PRIVATE Query {
			private:
				const char *path;						///< @brief Path for URL request.
				ResponseType type = ResponseType::None;	///< @brief Response type for this query.

			public:
				Query(const XML::Node &node) : path{Quark{node,"path",""}.c_str()} {
					if(!(path && *path != '/')) {
						throw std::runtime_error("Required argument 'path' is missing on invalid");
					}
				}

				inline operator ResponseType() const noexcept {
					return type;
				}

				bool operator==(const char *p) const noexcept {
					return strncasecmp(p,path,strlen(path)) == 0;
				}

			};

			std::vector<Query> queries;

		public:
			Module();
			virtual ~Module();

			// Udjat::Worker
			ResponseType probe(const Request &request) const noexcept override;
			bool work(Request &request, Response::Table &response) const override;
			bool work(Request &request, Response::Value &response) const override;

			// Udjat::Factory
			bool generic(const pugi::xml_node &node) override;
			std::shared_ptr<Abstract::Agent> AgentFactory(const Abstract::Object &parent, const pugi::xml_node &node) const;

		};

	}

 }
