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
  * @brief Implements sqlite3 session.
  */

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/string.h>
 #include <udjat/tools/value.h>
 #include <udjat/tools/report.h>
 #include <udjat/tools/logger.h>
 #include <udjat/tools/sql.h>
 #include <udjat/tools/timestamp.h>
 #include <udjat/agent/level.h>
 #include <private/cppdb.h>
 #include <cppdb/frontend.h>
 #include <mutex>
 #include <stdexcept>
 #include <ctime>

 using namespace std;

 namespace Udjat {

	SQL::Session::Session(const char *dbname) : cppdb::session{dbname} {
	}

	SQL::Session::~Session() {
	}

	void SQL::Session::get(cppdb::result &res, Udjat::Value &values) {

		// http://cppcms.com/sql/cppdb/classcppdb_1_1result.html
		for(int col = 0; col < res.cols();col++) {
			Value &value = values[res.name(col).c_str()];
			switch((Value::Type) value) {
			case Value::Undefined:
			case Value::String:
			case Value::Url:
			case Value::Icon:
				value.set(res.get<string>(col));
				break;

			case Value::Timestamp:
				{
					struct tm t;
					res.fetch(col,t);
					value.set(TimeStamp{t});
				}
				break;

			case Value::Signed:
				value.set(res.get<int>(col));
				break;

			case Value::Unsigned:
				value.set(res.get<unsigned int>(col));
				break;

			case Value::Real:
				value.set(res.get<double>(col));
				break;

			case Value::Boolean:
				value.set((bool) res.get<int>(col));
				break;

			case Value::Fraction:
				value.set(res.get<double>(col)/100);
				break;

			case Value::State:
				value.set(LevelFactory(res.get<string>(col).c_str()));
				break;

			default:
				{
					Logger::String{"Unexpected type '",std::to_string((Value::Type) value),"' on response data"}.warning();
					value.clear(Value::String);
					value.set(res.get<string>(col));
				}

			}

			debug("value(",res.name(col),")='",value.to_string().c_str(),"'");

		}
	}

	void SQL::Session::get(cppdb::result &res, Udjat::Report &report) {
		for(int col = 0; col < res.cols();col++) {
			if(res.is_null(col)) {
				report.push_back("");
			} else {
				std::string str;
				res.fetch(col,str);
				report.push_back(str);
			}
		}
	}

 }
