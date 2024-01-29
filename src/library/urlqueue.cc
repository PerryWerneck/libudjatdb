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
 #include <udjat/tools/sql/script.h>
 #include <udjat/tools/http/client.h>
 #include <private/urlqueue.h>
 #include <udjat/tools/value.h>
 #include <private/module.h>
 #include <memory>

 using namespace std;

 namespace Udjat {

	SQL::URLQueue::URLQueue(const XML::Node &node)
		:	SQL::Agent<size_t>(node),
			Udjat::Protocol{Quark(node,"url-queue-name","sql").c_str(),SQL::module_info},
			ins{node,"insert",true,false},
			send{node,"send",true,false},
			after_send{node,"after-send",true,false},
			send_interval{Object::getAttribute(node, "urlqueue", "send-interval", (unsigned int) 60)},
			send_delay{Object::getAttribute(node, "urlqueue", "send-delay", (unsigned int) 2)} {
	}

	SQL::URLQueue::~URLQueue() {
	}

	bool SQL::URLQueue::refresh(bool b) {

		debug("----------------- Refreshing url queue");

		// First refresh queue size.
		bool rc = SQL::Agent<size_t>::refresh(b);
		size_t qrecs = SQL::Agent<size_t>::get();

		if(qrecs) {

			string url;

			try {

				bool success = false;

				auto response = Udjat::Value::ObjectFactory();
				send.exec(*this,*response);
				rc = true;

				url = (*response)["url"].to_string();
				HTTP::Client client(url.c_str());

				switch(HTTP::MethodFactory((*response)["action"].to_string().c_str())) {
				case HTTP::Get:
					{
						auto response = client.get();
						SQL::Agent<size_t>::info() << url << endl;
						Logger::write(Logger::Trace,response);
						success = true;
					}
					break;

				case HTTP::Post:
					{
						auto result = client.post((*response)["payload"].to_string().c_str());
						Logger::write(Logger::Trace,result);
						success = true;
					}
					break;

				default:
					SQL::Agent<size_t>::error() << "Unexpected verb '" << (*response)["action"].to_string() << "' sending queued request, ignoring" << endl;
					success = false;
				}

				if(success) {
					after_send.exec(*this,*response);
					SQL::Agent<size_t>::set(--qrecs);
					if(send_interval) {
						sched_update(send_interval);
					}
				}

			} catch(const std::exception &e) {

				SQL::Agent<size_t>::error() << url << ": " << e.what() << endl;

			}

		}

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

				debug("----------------------------> Inserting alert on queue");
				(*value)["url"] = url().c_str(),
				(*value)["action"] = std::to_string(method()),
				(*value)["payload"] = payload(),

				agent->ins.exec(value);

				{
					URLQueue *obj = const_cast<URLQueue *>(this->agent);
					if(obj) {
						obj->SQL::Agent<size_t>::set(obj->SQL::Agent<size_t>::get()+1);
						debug("QUEUE set to ",obj->SQL::Agent<size_t>::get());
						obj->sched_update(1);	// Request agent update.
					} else {
						Logger::String{"Unable to request agent update"}.warning("urlqueue");
					}
				}

				debug("-------- Inserted");

				// Force as complete.
				progress(1,1);
				return "";
			}

		};

		return make_shared<Worker>(this);

	}


 }


