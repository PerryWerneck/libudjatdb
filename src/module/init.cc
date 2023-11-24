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

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/module.h>
 #include <udjat/worker.h>
 #include <stdexcept>

 using namespace Udjat;
 using namespace std;

 /// @brief Register udjat module.
 Udjat::Module * udjat_module_init() {

	static const Udjat::ModuleInfo module_info{"cppdb", "CPPDB SQL Module"};

	class Module : public Udjat::Module, private Udjat::Worker {
	public:
		Module() : Udjat::Module("cppdb",module_info), Udjat::Worker("cppdb",module_info) {
		};

		virtual ~Module() {
		}

		bool probe(const char *path) const noexcept override {

			debug("Probing '",path,"'");


			return false;
		}

		bool work(Request &request, Response &response) const override {

			return false;
		}

	};

	return new Module();

 }
