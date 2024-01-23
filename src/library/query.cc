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
 #include <private/module.h>
 #include <udjat/tools/xml.h>
 #include <udjat/tools/sql/query.h>

 namespace Udjat {

	SQL::Query::Query(const XML::Node &node) : path{Quark{node,"path",nullptr}.c_str()} {

	}

	bool SQL::Query::operator==(const char *p) const noexcept {
		size_t szpath = strlen(path);
		return strncasecmp(p,path,szpath) == 0 && p[szpath] == '/';
	}

	bool SQL::Query::work(Request &request, Response::Value &response) const {

		debug(__FUNCTION__,"('",request.path(),"')");

		return false;
	}

	bool SQL::Query::work(Request &request, Response::Table &response) const {

		debug(__FUNCTION__,"('",request.path(),"')");

		return false;
	}

 }

