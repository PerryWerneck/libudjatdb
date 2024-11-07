# Maintainer: Perry Werneck <perry.werneck@gmail.com>

# SPDX-License-Identifier: LGPL-3.0-or-later 

# Copyright (C) 2024 Perry Werneck <perry.werneck@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

project(
	'udjatdb', 
	['cpp'],
	version: '1.2.1',
	default_options : ['c_std=c11', 'cpp_std=c++17'],
	license: 'GPL-3.0-or-later',
)

project_description = 'Database library for udjat'

#
# Versioning
#
pkg_version = meson.project_version()
version_array = pkg_version.split('.')
pkg_major_version = version_array[0].to_int()
pkg_minor_version = version_array[1].to_int()
pkg_micro_version = version_array[2].to_int()

libversion = '@0@.@1@'.format(pkg_major_version,pkg_minor_version)

#
# i18n
#
#subdir('po')

#
# LibUdjat
#
libudjat = dependency('libudjat') 
lib_deps = [
  libudjat,
]

#
# Compiler flags
#
cxx = meson.get_compiler('cpp')
enable_debug = get_option('debug') or get_option('buildtype').contains('debug')

compiler_flags = [
  '-ggdb3',
  '-ffat-lto-objects',
  '-fvisibility=hidden',
  '-fPIC',
  '-DHAVE_CONFIG_H=1',
  '-DBUILD_DATE='+run_command('date','+%Y%m%d').stdout().strip(),
]

if enable_debug
  compiler_flags += [
	  '-DDEBUG=1'
  ]
else
  compiler_flags += [
	'-DNDEBUG=1'
  ]
endif

add_project_arguments(cxx.get_supported_arguments(compiler_flags), language: 'cpp')

#
# Configuration
#
app_conf = configuration_data()
app_conf.set_quoted('PACKAGE_NAME', meson.project_name())
app_conf.set_quoted('PACKAGE_VERSION', meson.project_version())
app_conf.set_quoted('PACKAGE_DESCRIPTION', project_description)
app_conf.set('PACKAGE_VERSION_MAJOR', pkg_major_version)
app_conf.set('PACKAGE_VERSION_MINOR', pkg_minor_version)
app_conf.set('PACKAGE_VERSION_MICRO', pkg_micro_version)

if cxx.compiles('#include <unistd.h>', name : 'unistd.h')
  app_conf.set('HAVE_UNISTD_H', 1)
endif

#
# SDK
#

includes_dir = include_directories('../src/include')

#
# Sources
#
lib_src = [
'src/library/alert.cc',
'src/library/apicall.cc',
'src/library/controller.cc',
'src/library/script.cc',
'src/library/urlqueue.cc',
'src/library/module.cc',
]

cppdb_src = [
  'src/library/engines/cppdb/alert.cc',
  'src/library/engines/cppdb/module.cc',
  'src/library/engines/cppdb/exec.cc',
  'src/library/engines/cppdb/statement.cc',
]

sqlite_src = [
  'src/library/engines/sqlite/alert.cc',
  'src/library/engines/sqlite/exec.cc',
  'src/library/engines/sqlite/module.cc',
  'src/library/engines/sqlite/session.cc',
  'src/library/engines/sqlite/statement.cc',
]

module_src = [
  'src/module/init.cc',
]

cppdb_test_src = [
'src/testprogram/cppdb.cc',
]

sqlite_test_src = [
'src/testprogram/sqlite.cc'
]

#
# Targets
#
config_src = [
  configure_file(
      output : 'config.h', 
      configuration : app_conf
  )
]

#
# SQLite Engine
#
sqlite = dependency('sqlite3') 

if sqlite.found()

  sqlite_deps = lib_deps + [ sqlite ]

  sqlite_dynamic = shared_library(
    'udjatsqlite', 
    config_src + lib_src + sqlite_src,
    install: true,
    version : libversion,
    soversion : libversion,
    gnu_symbol_visibility: 'hidden',
    dependencies: sqlite_deps,
    include_directories: includes_dir
  )

  static_library(
    'udjatsqlite', 
    config_src + lib_src,
    install: true,
    dependencies: sqlite_deps,
    include_directories: includes_dir
  )

  shared_library(
    'sqlite', 
    config_src + module_src + [ 'src/library/engines/sqlite/module.cc' ],
    name_prefix: 'udjat-module-',
    install: true,
    link_with: sqlite_dynamic,
    install_dir: libudjat.get_variable('module_path'),
    gnu_symbol_visibility: 'hidden',
    dependencies: sqlite_deps,
    include_directories: includes_dir
  )

  sqlite_conf = configuration_data()
  sqlite_conf.set('PACKAGE_NAME', 'libudjatsqlite')
  sqlite_conf.set('PACKAGE_VERSION', meson.project_version())
  sqlite_conf.set('PACKAGE_DESCRIPTION','SQLite based database library for udjat')
  sqlite_conf.set('REQUIRES','sqlite3')
  sqlite_conf.set('prefix',get_option('prefix'))
  sqlite_conf.set('exec_prefix',get_option('prefix'))
  sqlite_conf.set('includedir',get_option('includedir'))
  sqlite_conf.set('libdir',get_option('libdir'))

  if host_machine.system() == 'windows'
    sqlite_conf.set('DYNLIBS','-ludjatsqlite.dll')
    sqlite_conf.set('STATICLIBS','-ludjatsqlite')
  else
    sqlite_conf.set('DYNLIBS','-ludjatsqlite')
    sqlite_conf.set('STATICLIBS','-l:libudjatsqlite.a')
  endif
  
  configure_file(
    input : 'sdk/dynamic.pc.in',
    output : 'libudjatsqlite.pc',
    install: true,
    install_dir: get_option('libdir') + '/pkgconfig',
    configuration : sqlite_conf
  )

  configure_file(
    input : 'sdk/static.pc.in',
    output : 'libudjatsqlite-static.pc',
    install: true,
    install_dir: get_option('libdir') + '/pkgconfig',
    configuration : sqlite_conf
  )

endif

if cxx.compiles('#include <cppdb/defs.h>', name : 'cppdb')

  cppdb_dynamic = shared_library(
    'udjatcppdb', 
    config_src + lib_src + cppdb_src,
    install: true,
    version : libversion,
    soversion : libversion,
    dependencies: lib_deps,
    link_args: [ '-lcppdb' ],
    gnu_symbol_visibility: 'hidden',
    include_directories: includes_dir
  )

  static_library(
    'udjatcppdb', 
    config_src + lib_src + cppdb_src,
    install: true,
    dependencies: lib_deps,
    include_directories: includes_dir
  )

  shared_library(
    'cppdb', 
    config_src + module_src + [ 'src/library/engines/cppdb/module.cc' ],
    name_prefix: 'udjat-module-',
    install: true,
    link_with: cppdb_dynamic,
    install_dir: libudjat.get_variable('module_path'),
    gnu_symbol_visibility: 'hidden',
    dependencies: sqlite_deps,
    include_directories: includes_dir
  )

  cppdb_conf = configuration_data()
  cppdb_conf.set('PACKAGE_NAME', 'libudjatcppdb')
  cppdb_conf.set('PACKAGE_VERSION', meson.project_version())
  cppdb_conf.set('PACKAGE_DESCRIPTION','CppDB based database library for udjat')
  cppdb_conf.set('REQUIRES','cppdb')
  cppdb_conf.set('prefix',get_option('prefix'))
  cppdb_conf.set('exec_prefix',get_option('prefix'))
  cppdb_conf.set('includedir',get_option('includedir'))
  cppdb_conf.set('libdir',get_option('libdir'))

  if host_machine.system() == 'windows'
    cppdb_conf.set('DYNLIBS','-ludjatcppdb.dll')
    cppdb_conf.set('STATICLIBS','-ludjatcppdb')
  else
    cppdb_conf.set('DYNLIBS','-ludjatcppdb')
    cppdb_conf.set('STATICLIBS','-l:libudjatcppdb.a')
  endif
  
  configure_file(
    input : 'sdk/dynamic.pc.in',
    output : 'libudjatcppdb.pc',
    install: true,
    install_dir: get_option('libdir') + '/pkgconfig',
    configuration : cppdb_conf
  )

  configure_file(
    input : 'sdk/static.pc.in',
    output : 'libudjatcppdb-static.pc',
    install: true,
    install_dir: get_option('libdir') + '/pkgconfig',
    configuration : cppdb_conf
  )

endif

