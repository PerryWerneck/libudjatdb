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
 #include <udjat/tools/sql/script.h>
 #include <udjat/tools/intl.h>

 #include <iostream>
 #include <private/module.h>
 #include <private/urlqueue.h>

 using namespace std;

 namespace Udjat {

	namespace SQL {

		URLQueue::URLQueue(const XML::Node &node)
			:	SQL::Agent<size_t>(node),
				Udjat::Protocol{String(node,"url-queue-name","sql").as_quark(),SQL::module_info},
				ins{SQL::Script::parse(node,"insert",true)},
				get_values{SQL::Script::parse(node,"get",true)},
				after_send{SQL::Script::parse(node,"after-send",false)},
				send_interval{Object::getAttribute(node, "urlqueue", "send-interval", (unsigned int) 60)},
				send_delay{Object::getAttribute(node, "urlqueue", "send-delay", (unsigned int) 2)} {
		}

		URLQueue::~URLQueue() {
		}

		std::shared_ptr<Abstract::State> URLQueue::computeState() {

			// Get current value.
			size_t pending = SQL::Agent<size_t>::get();

			// Do we have XML defined states?
			for(auto state : states) {
				if(state->compare(pending))
					return state;
			}

			// No, we dont! Is the current state valid?
			{
				Udjat::State<size_t> *selected = dynamic_cast<Udjat::State<size_t> *>(this->state().get());
				if(selected && selected->compare(pending)) {
					return this->state();
				}
			}

			// We dont have a xml defined state and the current one is invalid, select an internal one.

			/// @brief Message based state.
			class StringState : public Udjat::State<size_t> {
			private:
				std::string message;

			public:
				StringState(const char *name, Level level, size_t value, const std::string &msg) : Udjat::State<size_t>(name,value,level), message{msg} {
					Object::properties.summary = message.c_str();
				}
			};

			const char * name = Protocol::c_str();

			if(!pending) {

				return make_shared<StringState>(
								"queue-empty",
								Level::unimportant,
								pending,
								Message{ _("{} output queue is empty"), name }
							);

			} else if(pending == 1) {

				return make_shared<StringState>(
								"one-queued",
								Level::warning,
								pending,
								Message{ _("One pending request in the {} queue"), name }
							);

			}

			return make_shared<StringState>(
							"has-queued",
							Level::warning,
							pending,
							Message{ _("{} pending requests in the {} queue"), pending, name }
						);

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


