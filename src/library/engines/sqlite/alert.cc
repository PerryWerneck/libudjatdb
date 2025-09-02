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

/*
 #include <config.h>
 #include <udjat/tools/sql/script.h>
 #include <udjat/tools/abstract/object.h>
 #include <udjat/alert/abstract.h>
 #include <udjat/alert/activation.h>
 #include <udjat/alert/sql.h>
 #include <udjat/tools/value.h>
 #include <memory>
 #include <unistd.h>
 #include <udjat/tools/sql/script.h>
 #include <sqlite3.h>
 #include <private/sqlite.h>

 using namespace std;

 namespace Udjat {

	std::shared_ptr<Udjat::Alert::Activation> SQL::Alert::ActivationFactory() const {

		/// @brief SQL based alert activation.
		class Activation : public Udjat::Alert::Activation {
		private:

			const char *dburl;

			struct Statement {

				const char *text;

				struct Parameter {
					const char *name;
					string value;
					bool valid = false;

					Parameter(const char *n) : name{n} {
					}

				};

				std::vector<Parameter> parameters;

				Statement(const SQL::Statement &script) : text{script.text} {
					for(const auto &parm : script.parameter_names) {
						parameters.emplace_back(parm);
					}
				}

			};

			/// @brief The SQL statements.
			std::vector<Statement> statements;

			/// @brief Script results.
			std::shared_ptr<Value> results;

		public:
			Activation(const Abstract::Alert *alert, const SQL::Script &script)
				: Udjat::Alert::Activation{alert}, dburl{script.dbconn()}, results{Udjat::Value::ObjectFactory()} {

				for(const auto &from : script) {
					statements.emplace_back(from);
				}

			}

			void emit() override {

				if(Logger::enabled(Logger::Trace)) {
					Logger::String{"Emitting alert"}.trace(name.c_str());
				}

				// Execute statements
				{
					SQL::Session session{dburl};

					for(auto &statement : statements) {

						if(Logger::enabled(Logger::Debug)) {
							Logger::String{statement.text}.write(Logger::Debug,name.c_str());
						}

						auto stmt = session.prepare(statement.text);

						try {

							int column = 1;
							for(auto &parameter : statement.parameters) {
								string rvalue;
								if(results->getProperty(parameter.name,rvalue)) {
									debug(parameter.name,"= '",parameter.value,"' (from result)");
									session.check(
										sqlite3_bind_text(
											stmt,
											column,
											rvalue.c_str(),
											rvalue.size()+1,
											SQLITE_TRANSIENT
										)
									);
								} else if(parameter.valid) {
									debug(parameter.name,"= '",parameter.value,"' (from parameters)");
									session.check(
										sqlite3_bind_text(
											stmt,
											column,
											parameter.value.c_str(),
											parameter.value.size()+1,
											SQLITE_TRANSIENT
										)
									);
								} else {
									throw runtime_error(Logger::String{"Required parameter '",parameter.name,"' is missing"});
								}
								column++;
							}

							session.step(stmt, *results);

						} catch(...) {
							sqlite3_finalize(stmt);
							throw;
						}
						sqlite3_finalize(stmt);

					}

				}

			}

			Udjat::Alert::Activation & set(const Abstract::Object &object) override {

				for(auto &statement : statements) {
					for(auto &parameter : statement.parameters) {
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
*/

