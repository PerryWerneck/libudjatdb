/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) 2021 Perry Werneck <perry.werneck@gmail.com>
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
 #include <udjat/tools/sql/script.h>
 #include <udjat/tools/sql.h>
 #include <udjat/tools/value.h>
 #include <udjat/tools/logger.h>
 #include <udjat/tools/http/mimetype.h>
 #include <iostream>

 using namespace std;
 using namespace Udjat;

 static void test_sqlite() {

	debug("Testing SQLite...");

	SQL::Script script{

		"create table if not exists alerts (id integer primary key, inserted timestamp default CURRENT_TIMESTAMP, url text, action text, payload text);\n" \
		"insert into alerts (url,action,payload) values (${url},${action},${payload});" 
	};

	Value request, response;
	request["url"] = "http://localhost";
	request["action"] = "+";
	request["payload"] = "";
	script.exec("/tmp/test.sqlite",request,response);

	SQL::Script{
		"select * from alerts" 
	}.exec("/tmp/test.sqlite",request,response);

	cout << endl;
	response.serialize(cout,MimeType::xml);
	cout << endl;
 }

 static void test_cppdb() {

	debug("Testing CPPDB...");

	SQL::Script script{

		"create table if not exists alerts (id integer primary key, inserted timestamp default CURRENT_TIMESTAMP, url text, action text, payload text);\n" \
		"insert into alerts (url,action,payload) values (${url},${action},${payload});" 
	};

	Value request, response;
	request["url"] = "http://localhost";
	request["action"] = "+";
	request["payload"] = "";
	script.exec("sqlite3:db=/tmp/test.sqlite",request,response);

	SQL::Script{
		"select * from alerts" 
	}.exec("sqlite3:db=/tmp/test.sqlite",request,response);

	cout << endl;
	response.serialize(cout,MimeType::xml);
	cout << endl;
 }

 int main(int argc, char **argv) {

	Logger::verbosity(9);
	Logger::console(true);
	Logger::redirect();
	
	test_cppdb();

	return 0;
 }

/*
 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tests.h>
 #include <udjat/moduleinfo.h>
 #include <udjat/module.h>
 #include <udjat/tools/sql/script.h>
 #include <udjat/tools/string.h>
 #include <udjat/tests.h>
 #include <udjat/tools/application.h>

 using namespace std;
 using namespace Udjat;

 int main(int argc, char **argv) {

	static const ModuleInfo info{"civetweb-tester"};
	
	return Testing::run(argc,argv,info,[](Application &){

	 	udjat_module_init();

	}, String{SQL::engine(),".xml"}.c_str());

 }
 
*/
