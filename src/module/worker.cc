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
  * @brief Implements SQL Worker.
  */

 #include <config.h>
 #include <udjat/defs.h>
 #include <private/module.h>
 #include <udjat/worker.h>
 #include <udjat/tools/logger.h>

 using namespace std;

 namespace Udjat {

	Worker::ResponseType SQL::Module::probe(const Request &request) const noexcept  {

		debug(__FUNCTION__,"('",request.path(),"')");

		for(const auto &query : queries) {
			if(query == request.path()) {
				return (Worker::ResponseType) query;
			}
		}

		return Worker::None;
	}

	bool SQL::Module::work(Request &request, Response::Value &response) const {

		debug(__FUNCTION__,"('",request.path(),"')");

		for(const auto &query : queries) {
			if(query == request.path()) {
				return query.work(request,response);
			}
		}

		return false;
	}

	bool SQL::Module::work(Request &request, Response::Table &response) const {

		debug(__FUNCTION__,"('",request.path(),"')");

		for(const auto &query : queries) {
			if(query == request.path()) {
				return query.work(request,response);
			}
		}

		return false;
	}

 }


