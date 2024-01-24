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
  * @brief Brief description of this source.
  */

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/xml.h>
 #include <udjat/tools/sql/query.h>
 #include <udjat/tools/logger.h>
 #include <udjat/tools/timestamp.h>

 namespace Udjat {

	SQL::Query::Query(const XML::Node &node)
		:	SQL::Statement{node},
			path{Quark{node,"path",nullptr}.c_str()},
			method{HTTP::MethodFactory(node,"action","get")},
			expires{(time_t) TimeStamp{node,"expires",(time_t) 300}} {
	}

	bool SQL::Query::operator==(const char *p) const noexcept {
		size_t szpath = strlen(path);
		return (strncasecmp(p,path,szpath) == 0) && (p[szpath] == '/' || p[szpath] == 0);
	}

	void SQL::Query::head(const Request &request, Abstract::Response &response) const {
		debug(__FUNCTION__,"('",request.path(),"')");
		if(expires) {
			response.expires(time(0)+expires);
		}
	}

	bool SQL::Query::exec(Request &request, Response::Value &response) const {

		head(request,response);

		return false;
	}

	bool SQL::Query::exec(Request &request, Response::Table &response) const {

		head(request,response);

		return false;
	}

 }

