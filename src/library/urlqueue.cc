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
 #include <udjat/defs.h>
 #include <udjat/tools/xml.h>
 #include <udjat/agent.h>
 #include <udjat/agent/sql.h>
 #include <udjat/tools/protocol.h>

 #include <iostream>
 #include <private/module.h>
 #include <private/urlqueue.h>

 using namespace std;

 namespace Udjat {

	namespace SQL {

		URLQueue::URLQueue(const XML::Node &node)
			:	SQL::Agent<size_t>(node),
				Udjat::Protocol{String(node,"url-queue-name","sql").as_quark(),SQL::module_info},
				ins{SQL::Script::parse(node.child("insert"))},
				get_values{SQL::Script::parse(node.child("get"))},
				after_send{SQL::Script::parse(node.child("after-send"))},
				send_interval{Object::getAttribute(node, "urlqueue", "send-interval", (unsigned int) 60)},
				send_delay{Object::getAttribute(node, "urlqueue", "send-delay", (unsigned int) 2)} {
		}

		URLQueue::~URLQueue() {
		}

		bool URLQueue::refresh(bool b) {

			bool changed = SQL::Agent<size_t>::refresh(b);
			size_t count = Udjat::Agent<size_t>::get();
			if(!count) {
				return changed;
			}

			if(count) {

				try {

					send();

				} catch(std::exception &e) {

					SQL::Agent<size_t>::error() << e.what() << endl;

				}
			}

			return true;
		}

		std::shared_ptr<Protocol::Worker> URLQueue::WorkerFactory() const {

			class Worker : public Udjat::Protocol::Worker {
			private:
				URLQueue &agent;

			public:
				Worker(SQL::URLQueue *a) : agent{*a} {
				}

				virtual ~Worker() {
				}

				String get(const std::function<bool(double current, double total)> &progress) override {

					progress(0,0);

					Udjat::Value value;
					value["url"] = url().c_str();
					value["action"] = std::to_string(method());
					value["payload"] = payload();

					SQL::Script{agent.ins}.exec(agent.dbname,value);

					agent.set(agent.Udjat::Agent<size_t>::get()+1);
					agent.sched_update(1);

					// Force as complete.
					progress(1,1);
					return "";
				}

			};

			return make_shared<Worker>(const_cast<SQL::URLQueue *>(this));

		}

	}

 }


