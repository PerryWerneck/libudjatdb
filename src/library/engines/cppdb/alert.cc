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
  * @brief Implements SQL alert.
  */

 #include <config.h>
 #include <udjat/tools/sql/statement.h>
 #include <udjat/tools/abstract/object.h>
 #include <udjat/alert/abstract.h>
 #include <udjat/alert/activation.h>
 #include <udjat/alert/sql.h>
 #include <udjat/tools/value.h>
 #include <private/cppdb.h>
 #include <memory>
 #include <cppdb/frontend.h>
 #include <unistd.h>

 using namespace std;

 namespace Udjat {

	std::shared_ptr<Udjat::Alert::Activation> SQL::Alert::ActivationFactory() const {

		/// @brief SQL based alert activation.
		class Activation : public Udjat::Alert::Activation {
		private:

			const char *dburl;

			struct Script {

				const char *text;

				struct Parameter {
					const char *name;
					string value;
					bool valid = false;

					Parameter(const char *n) : name{n} {
					}

				};

				std::vector<Parameter> parameters;

				Script(const SQL::Statement::Script &script) : text{script.text} {
					for(const auto &parm : script.parameter_names) {
						parameters.emplace_back(parm);
					}
				}

			};

			std::vector<Script> scripts;

			/// @brief Script results.
			std::shared_ptr<Value> results;

		public:
			Activation(const Abstract::Alert *alert, const SQL::Statement &statement)
				: Udjat::Alert::Activation{alert}, dburl{statement.dbconn()}, results{Udjat::Value::ObjectFactory()} {

				for(const auto &from : statement) {
					scripts.emplace_back(from);
				}

			}

			void emit() override {

				if(Logger::enabled(Logger::Trace)) {
					Logger::String{"Emitting alert"}.trace(name.c_str());
				}

				// Execute scripts
				{
					cppdb::session session{dburl};
					cppdb::transaction guard(session);

					for(auto &script : scripts) {

						if(Logger::enabled(Logger::Debug)) {
							Logger::String{script.text}.write(Logger::Debug,name.c_str());
						}

						auto stmt = session.create_statement(script.text);
						for(auto &parameter : script.parameters) {
							string rvalue;
							if(results->getProperty(parameter.name,rvalue)) {
								debug(parameter.name,"= '",parameter.value,"' (from result)");
								stmt.bind(rvalue);
							} else if(parameter.valid) {
								debug(parameter.name,"= '",parameter.value,"' (from parameters)");
								stmt.bind(parameter.value);
							} else {
								throw runtime_error(Logger::String{"Required parameter '",parameter.name,"' is missing"});
							}
						}

						if(strncasecmp(script.text,"select",6)) {

							// Not a select, just execute.
							stmt.exec();

						} else {

							// It's a select, store results.
							auto row = stmt.row();
							for(int col = 0; col < row.cols();col++) {
								string val;
								row.fetch(col,val);
								(*results)[row.name(col).c_str()] = val.c_str();
							}

						}

					}

					guard.commit();

				}

			}

			Udjat::Alert::Activation & set(const Abstract::Object &object) override {

				for(auto &script : scripts) {
					for(auto &parameter : script.parameters) {
						parameter.valid = parameter.valid || object.getProperty(parameter.name,parameter.value);
					}
				}

				return *this;
			}

			Udjat::Value & getProperties(Udjat::Value &value) const {
				results->getProperties(value);
				return Udjat::Alert::Activation::getProperties(value);
			}

		};

		return make_shared<Activation>(this,script);

	}

 }

