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
  * @brief Implements URL Queue send().
  */

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/http/client.h>

 #include <iostream>
 #include <private/module.h>
 #include <private/urlqueue.h>
 #include <private/sqlite.h>

 #include <sqlite3.h>

 using namespace std;

 namespace Udjat {

	void SQL::URLQueue::send() {

		Session db{dbname};

		Udjat::Value values;
		db.exec(get_values,values,values);

		HTTP::Client client(values["url"].c_str());

		switch(HTTP::MethodFactory(values["action"].c_str())) {
		case HTTP::Get:
			{
				auto result = client.get();
				Logger::write(Logger::Trace,result);
			}
			break;

		case HTTP::Post:
			{
				auto result = client.post(values["payload"].c_str());
				Logger::write(Logger::Trace,result);
			}
			break;

		default:
			throw runtime_error("Unsupported HTTP verb");
		}

		size_t count = Udjat::Agent<size_t>::get();

		db.exec(after_send,values,values);
		count--;

		Udjat::Agent<size_t>::set(count);

		if(send_interval) {
			debug("Will send next in ",send_interval," second(s)");
			sched_update(send_interval);
		}

	}

 }
