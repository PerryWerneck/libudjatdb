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
  * @brief Implements URL Queue agent.
  */

 #include <config.h>
 #include <udjat/tools/xml.h>
 #include <udjat/tools/quark.h>
 #include <udjat/agent/sql.h>
 #include <udjat/module/info.h>
 #include <udjat/tools/protocol.h>
 #include <udjat/tools/sql/statement.h>
 #include <private/urlqueue.h>
 #include <udjat/tools/value.h>
 #include <memory>

 using namespace std;

 namespace Udjat {

	static const ModuleInfo module_info{"cppdb", "CPPDB URL Queue Agent"};

	SQL::URLQueue::URLQueue(const XML::Node &node)
		:	SQL::Agent<size_t>(node),
			Udjat::Protocol{Quark(node,"name","sql").c_str(),module_info},
			ins{node,"insert",true,false},
			send{node,"send",true,false},
			after_send{node,"after-send",true,false},
			send_delay{Object::getAttribute(node, "sqlite", "retry-delay", (unsigned int) 1)} {
	}

	SQL::URLQueue::~URLQueue() {
	}

	bool SQL::URLQueue::refresh(bool b) {

		// First refresh queue size.
		bool rc = SQL::Agent<size_t>::refresh(b);



		return rc;
	}

	std::shared_ptr<Protocol::Worker> SQL::URLQueue::WorkerFactory() const {

		class Worker : public Udjat::Protocol::Worker {
		private:
			const URLQueue *agent;

		public:
			Worker(const SQL::URLQueue *a) : agent{a} {
			}

			virtual ~Worker() {
			}

			String get(const std::function<bool(double current, double total)> &progress) override {

				progress(0,0);

				auto value = Udjat::Value::ObjectFactory();

				(*value)["url"] = url().c_str(),
				(*value)["method"] = std::to_string(method()),
				(*value)["payload"] = payload(),

				agent->ins.exec(value);

				{
					URLQueue *obj = const_cast<URLQueue *>(this->agent);
					if(obj) {
						obj->sched_update(1);	// Request agent update.
					} else {
						Logger::String{"Unable to request agent update"}.warning("urlqueue");
					}
				}

				// Force as complete.
				progress(1,1);
				return "";
			}

		};

		return make_shared<Worker>(this);

	}


 }


