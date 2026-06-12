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
	version: '2.2.1',
	default_options : ['cpp_std=c++17', 'buildtype=release' ],
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
lib_deps = [
]

#
# LibUdjat
#
libudjat = dependency(
  'libudjat',
  version: '>=2.7.0',
  fallback: ['libudjat', 'static_library'],
)

#
# Compiler flags
#
cxx = meson.get_compiler('cpp')
enable_debug = get_option('debug') or get_option('buildtype').contains('debug')

time_epoch = run_command('sh', '-c', 'echo "$SOURCE_DATE_EPOCH"', check : true).stdout().strip()
if time_epoch == ''
  time_epoch = run_command('date','+%Y%m%d', check: true).stdout().strip()
endif

compiler_flags = [
  '-ggdb3',
  '-fvisibility=hidden',
  '-fPIC',
  '-DHAVE_CONFIG_H=1',
  '-Wno-overloaded-virtual',
  '-DBUILD_DATE='+time_epoch,
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

app_conf.set('PRODUCT_NAME', libudjat.get_variable('product_name'))
app_conf.set('PRODUCT_VERSION', libudjat.version())

if cxx.compiles('#include <unistd.h>', name : 'unistd.h')
  app_conf.set('HAVE_UNISTD_H', 1)
endif

#
# SDK
#
pkg = import('pkgconfig')
includes_dir = include_directories('src/include')

#
# Sources
#
lib_src = [
  'src/library/action.cc',
  'src/library/alert.cc',
  'src/library/script.cc',
  'src/library/urlqueue.cc',
  'src/library/module.cc',
]

cppdb_src = [
  'src/library/engines/cppdb/alert.cc',
  'src/library/engines/cppdb/session.cc',
  'src/library/engines/cppdb/module.cc',
  'src/library/engines/cppdb/exec.cc',
  'src/library/engines/cppdb/urlqueue.cc',
]

sqlite_src = [
  'src/library/engines/sqlite/alert.cc',
  'src/library/engines/sqlite/session.cc',
  'src/library/engines/sqlite/module.cc',
  'src/library/engines/sqlite/exec.cc',
  'src/library/engines/sqlite/urlqueue.cc',
]

module_src = [
  'src/module/init.cc',
]

test_src = [
'src/testprogram/testprogram.cc',
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
# OS stuff
#
if host_machine.system() == 'windows'

  # https://mesonbuild.com/Windows-module.html
  windows = import('windows')
  resources = windows.compile_resources(
    configure_file(
      input : 'src/library/resources.rc.in',
      output : 'resources.rc',
      configuration : app_conf
    )
  )

  lib_src += resources
  module_src += resources

endif

#
# SQLite Engine
#
sqlite = dependency('sqlite3', required: false) 

if sqlite.found()

  sqlite_deps = [ libudjat, sqlite ]

  sqlite_dynamic = shared_library(
    'udjatsqlite', 
    config_src + lib_src + sqlite_src,
    install: true,
    version : libversion,
    soversion : libversion,
    cpp_args: [ '-DSQLITE_BACKEND', '-ffat-lto-objects' ],
    gnu_symbol_visibility: 'hidden',
    dependencies: sqlite_deps,
    include_directories: includes_dir
  )

  static_library(
    'udjatsqlite', 
    config_src + lib_src,
    install: true,
    dependencies: sqlite_deps,
    cpp_args: [ '-DSQLITE_BACKEND', '-DSTATIC_LIBRARY', '-fno-lto' ],
    include_directories: includes_dir
  )

  sqlite_module = shared_library(
    'sqlite', 
    config_src + module_src + [ 'src/library/engines/sqlite/module.cc' ],
    name_prefix: 'udjat-module-',
    cpp_args: [ '-DSQLITE_BACKEND', '-ffat-lto-objects' ],
    install: true,
    install_tag: 'module',
    link_with: sqlite_dynamic,
    install_dir: libudjat.get_variable('module_path'),
    gnu_symbol_visibility: 'hidden',
    dependencies: sqlite_deps,
    include_directories: includes_dir
  )

  executable(
    'sqlite-test',
    config_src + test_src,
    install: false,
    cpp_args: [ '-DSQLITE_BACKEND' ],
    link_with : [ sqlite_dynamic, sqlite_module ],
    dependencies: [ libudjat ],
    include_directories: includes_dir
  )

  if host_machine.system() == 'windows'

    pkg.generate(
      name: 'libudjatsqlite',
      requires: lib_deps,
      requires_private: [ 'libudjat', sqlite ],
      description: project_description,
      libraries: [ '-ludjatsqlite.dll' ]
    )

  else

    pkg.generate(
      name: 'libudjatsqlite',
      requires: lib_deps,
      requires_private: [ 'libudjat', sqlite ],
      description: project_description,
      libraries: [ '-ludjatsqlite' ]
    )

  endif
  
endif

if cxx.compiles('#include <cppdb/defs.h>', name : 'cppdb')

  cppdb_dynamic = shared_library(
    'udjatcppdb', 
    config_src + lib_src + cppdb_src,
    install: true,
    version : libversion,
    soversion : libversion,
    dependencies: [ libudjat ],
    cpp_args: [ '-DCPPDB_BACKEND', '-ffat-lto-objects' ],
    link_args: [ '-lcppdb' ],
    gnu_symbol_visibility: 'hidden',
    include_directories: includes_dir
  )

  static_library(
    'udjatcppdb', 
    config_src + lib_src + cppdb_src,
    install: true,
    dependencies: lib_deps + [ libudjat ],
    cpp_args: [ '-DCPPDB_BACKEND', '-DSTATIC_LIBRARY', '-fno-lto' ],
    include_directories: includes_dir
  )

  cppdb_module = shared_library(
    'cppdb', 
    config_src + module_src + [ 'src/library/engines/cppdb/module.cc' ],
    name_prefix: 'udjat-module-',
    install: true,
    link_with: cppdb_dynamic,
    dependencies: [ libudjat ],
    cpp_args: [ '-DCPPDB_BACKEND', '-ffat-lto-objects' ],
    install_dir: libudjat.get_variable('module_path'),
    gnu_symbol_visibility: 'hidden',
    include_directories: includes_dir
  )

  if host_machine.system() == 'windows'

    pkg.generate(
      name: 'libudjatcppdb',
      description: project_description,
      libraries: [ '-ludjatcppdb.dll' ],
      libraries_private: [ '-lcppdb' ],
    )

  else

    pkg.generate(
      name: 'libudjatcppdb',
      description: project_description,
      libraries: [ '-ludjatcppdb' ],
      libraries_private: [ '-lcppdb' ],
    )

  endif
  
  executable(
    'cppdb-test',
    config_src + test_src,
    install: false,
    link_with : [ cppdb_dynamic, cppdb_module ],
    cpp_args: [ '-DCPPDB_BACKEND' ],
    dependencies: [ libudjat ],
    include_directories: includes_dir
  )

endif

install_headers(
  'src/include/udjat/agent/sql.h',
  subdir: 'udjat/agent/sql'  
)

install_headers(
  'src/include/udjat/alert/sql.h',
  subdir: 'udjat/alert/sql'  
)

install_headers(
  'src/include/udjat/tools/sql.h',
  subdir: 'udjat/tools'  
)

install_headers(
  'src/include/udjat/tools/sql/script.h',
  'src/include/udjat/tools/sql/module.h',
  subdir: 'udjat/tools/sql'  
)

install_headers(
  'src/include/udjat/tools/actions/sql.h',
  subdir: 'udjat/tools/actions'  
)
