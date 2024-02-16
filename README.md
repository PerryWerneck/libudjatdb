# Database module for udjat

Database module for udjat using cppdb or sqlite as backend.

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
![CodeQL](https://github.com/PerryWerneck/udjat-module-database/workflows/CodeQL/badge.svg?branch=master)
[![build result](https://build.opensuse.org/projects/home:PerryWerneck:udjat/packages/udjat-module-database/badge.svg?type=percent)](https://build.opensuse.org/package/show/home:PerryWerneck:udjat/udjat-module-database)

## Using module

### Examples

[Udjat](../../../udjat) service configuration to store an [user module](../../../udjat-module-users) alert on sqlite database:

```xml
<?xml version="1.0" encoding="UTF-8" ?>
<config log-debug='yes' log-trace='yes'>

	<!-- The HTTP module implements the http client backend -->
	<module name='http' required='yes' />
	
	<!-- Implements user's monitor -->
	<module name='users' required='yes' />
	
	<!-- Load SQLite module -->
	<module name='sqlite' required='yes' />	

	<!-- SQLite database -->
	<attribute name='sqlite-file' value='/tmp/alerts.db' />
	
	<!-- Special type of SQL Agent keeping a queue of URLs -->
	<agent type='sql' name='alerts' url-queue-name='sql' update-timer='60' >
	
		<attribute name='summary' value='Alerts on queue' />
		<attribute name='label' value='Alert queue' />
	
		<!-- Initialize an URL queue -->
		<init>
			create table if not exists alerts (id integer primary key, inserted timestamp default CURRENT_TIMESTAMP, url text, action text, payload text)
		</init>
		
		<!-- Get agent value -->
		<refresh>
			select count (*) as value from alerts
		</refresh>

		<!-- Store URL on database -->
		<insert>
			insert into alerts (url,action,payload) values (${url},${action},${payload})
		</insert>

		<!-- Get data to send -->
		<send>
			select id,url,action,payload from alerts limit 1
		</send>
		
		<!-- Delete ID after sending it -->
		<after-send>
			delete from alerts where id=${id}
		</after-send>
	
	</agent>
	
	<!-- Declare an user monitor agent -->
	<users name='users' update-timer='60'>

		<!-- The URL is prefixed with 'sqlite+' to be stored on database -->
		<alert name='logout' event='logout' max-retries='1' action='post' url='sqlite+http://localhost'>
			{"user":"${username}","macaddress":"${macaddress}"}
		</alert>

	</users>
	
</config>
```

