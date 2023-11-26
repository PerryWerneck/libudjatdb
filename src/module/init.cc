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
 #include <udjat/factory.h>
 #include <stdexcept>

 #include <private/module.h>

 using namespace Udjat;
 using namespace std;

 namespace Udjat {

	static const ModuleInfo module_info{"cppdb", "CPPDB SQL Module"};

	SQL::Module::Module() : Udjat::Module("cppdb",module_info), Udjat::Worker("cppdb",module_info), Udjat::Factory("sql",module_info) {
	};

	SQL::Module::~Module() {
	}

 }

 /// @brief Register udjat module.
 Udjat::Module * udjat_module_init() {

	return new SQL::Module();

 }
