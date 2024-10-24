project(
	'udjatdb', 
	['cpp'],
	version: '2.1.0',
	default_options : ['c_std=c11', 'cpp_std=c++17'],
	license: 'GPL-3.0-or-later',
)

project_description = 'Database module and library for udjat'

#
# options
#
build_cppdb = get_option('build_cppdb')
build_sqlite = get_option('build_sqlite')

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

compiler_flags_common = [
  '-fvisibility=hidden',
  '-fPIC'
	]

if enable_debug
  compiler_flags_common += [
	  '-DDEBUG=1'
  ]
else
  compiler_flags_common += [
	'-DNDEBUG=1'
  ]
endif

compiler_flags_common += [
  '-DHAVE_CONFIG_H=1'
]

global_cxxflags = cxx.get_supported_arguments(compiler_flags_common)

add_project_arguments(global_cxxflags, language: 'cpp')

add_project_arguments(
            cxx.get_supported_arguments(
                [
                    '-ggdb3',
                    '-ffat-lto-objects',
                    '-fvisibility=hidden',
                    '-fPIC',
                    '-DBUILD_DATE='+run_command('date','+%Y%m%d').stdout().strip(),
                ],
            ),
            language: 'cpp',
        )

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

config_src = [
  configure_file(
      output : 'config.h', 
      configuration : app_conf
  )
]

#
# SDK
#
cppdb_conf = configuration_data()
cppdb_conf.set('PACKAGE_NAME', 'udjatcppdb')
cppdb_conf.set('PACKAGE_VERSION', meson.project_version())
cppdb_conf.set('PACKAGE_DESCRIPTION','Database module and library for udjat with cppdb backend')
cppdb_conf.set('prefix',get_option('prefix'))
cppdb_conf.set('exec_prefix',get_option('prefix'))
cppdb_conf.set('includedir',get_option('includedir'))
cppdb_conf.set('libdir',get_option('libdir'))

sqlite_conf = configuration_data()
sqlite_conf.set('PACKAGE_NAME', 'udjatsqlite')
sqlite_conf.set('PACKAGE_VERSION', meson.project_version())
sqlite_conf.set('PACKAGE_DESCRIPTION', 'Database module and library for udjat with SQLite backend')
sqlite_conf.set('prefix',get_option('prefix'))
sqlite_conf.set('exec_prefix',get_option('prefix'))
sqlite_conf.set('includedir',get_option('includedir'))
sqlite_conf.set('libdir',get_option('libdir'))

includes_dir = include_directories('../src/include')

#
# Dependencies
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
  'src/library/engines/cppdb/exec.cc',
  'src/library/engines/cppdb/statement.cc',
  'src/library/engines/cppdb/module.cc',
]

sqlite_src = [
  'src/library/engines/sqlite/alert.cc',
  'src/library/engines/sqlite/exec.cc',
  'src/library/engines/sqlite/module.cc',
  'src/library/engines/sqlite/session.cc',
  'src/library/engines/sqlite/statement.cc',
  'src/library/engines/sqlite/module.cc',
]

module_src = [
  'src/module/init.cc',
  'src/library/engines/cppdb/module.cc',
  'src/library/urlqueue.cc',
]

#
# CPPDB Targets
#
if build_cppdb
  cppdblib = shared_library(
    'udjatcppdb', 
    config_src + lib_src + cppdb_src,
	  install: true,
	  version : libversion,
	  soversion : libversion,
	  gnu_symbol_visibility: 'hidden',
	  link_args : ['-lcppdb'],
	  dependencies: lib_deps,
	  include_directories: includes_dir
  )

  static_library(
    'udjatcppdb', 
    config_src + lib_src + cppdb_src,
	  install: true,
	  dependencies: lib_deps,
	  include_directories: includes_dir
  )

  # https://mesonbuild.com/Reference-manual_functions.html#shared_library
  cppdbmodule = shared_library(
    'cppdb', 
    config_src + module_src,
    name_prefix: 'udjat-module-',
	  install: true,
	  link_with: cppdblib,
	  install_dir: libudjat.get_variable('module_path'),
	  gnu_symbol_visibility: 'hidden',
	  dependencies: lib_deps,
	  include_directories: includes_dir
  )

  executable(
	  'cppdbtest',
	  config_src + [ 'src/testprogram/cppdb.cc' ],
    install: false,
	  link_with : [ cppdblib, cppdbmodule ],
	  dependencies: lib_deps,
	  include_directories: includes_dir
  )

  cppdb_conf.set('REQUIRES','')
  if host_machine.system() == 'windows'
    cppdb_conf.set('DYNLIBS','-ludjatcppdb.dll')
    cppdb_conf.set('STATICLIBS','-ludjatcppdb -lcppdb')
  else
    cppdb_conf.set('DYNLIBS','-ludjatcppdb')
    cppdb_conf.set('STATICLIBS','-l:libudjatcppdb.a -lcppdb')
  endif

  configure_file(
    input : 'sdk/dynamic.pc.in',
    output : 'udjatcppdb.pc',
    install: true,
    install_dir: get_option('libdir') + '/pkgconfig',
    configuration : cppdb_conf
  )

  configure_file(
    input : 'sdk/static.pc.in',
    output : 'udjatcppdb-static.pc',
    install: true,
    install_dir: get_option('libdir') + '/pkgconfig',
    configuration : cppdb_conf
  )

endif

#
# SQLITE Targets
#
if build_sqlite
  sqllib = shared_library(
    'udjatsqlite', 
    config_src + lib_src + sqlite_src,
	  install: true,
	  version : libversion,
	  soversion : libversion,
	  gnu_symbol_visibility: 'hidden',
	  link_args : ['-lcppdb'],
	  dependencies: lib_deps + [ dependency('sqlite3') ],
	  include_directories: includes_dir
  )

  static_library(
    'udjatsqlite', 
    config_src + lib_src + sqlite_src,
	  install: true,
	  dependencies: lib_deps,
	  include_directories: includes_dir
  )

  # https://mesonbuild.com/Reference-manual_functions.html#shared_library
  sqlmodule = shared_library(
    'sqlite', 
    config_src + module_src,
    name_prefix: 'udjat-module-',
	  install: true,
	  link_with: cppdblib,
	  install_dir: libudjat.get_variable('module_path'),
	  gnu_symbol_visibility: 'hidden',
	  dependencies: lib_deps,
	  include_directories: includes_dir
  )

  executable(
	  'sqlitetest',
	  config_src + [ 'src/testprogram/sqlite.cc' ],
    install: false,
	  link_with : [ sqllib, sqlmodule ],
	  dependencies: lib_deps,
	  include_directories: includes_dir
  )

  sqlite_conf.set('REQUIRES','sqlite3')
  if host_machine.system() == 'windows'
    sqlite_conf.set('DYNLIBS','-ludjatsqlite.dll')
    sqlite_conf.set('STATICLIBS','-ludjatsqlite')
  else
    sqlite_conf.set('DYNLIBS','-ludjatsqlite')
    sqlite_conf.set('STATICLIBS','-l:libudjatsqlite.a')
  endif
  
  configure_file(
    input : 'sdk/dynamic.pc.in',
    output : 'udjatsqlite.pc',
    install: true,
    install_dir: get_option('libdir') + '/pkgconfig',
    configuration : sqlite_conf
  )

  configure_file(
    input : 'sdk/static.pc.in',
    output : 'udjatsqlite-static.pc',
    install: true,
    install_dir: get_option('libdir') + '/pkgconfig',
    configuration : sqlite_conf
  )

endif

#
# Common targets
#
install_subdir('src/include/udjat', install_dir : get_option('includedir'))
