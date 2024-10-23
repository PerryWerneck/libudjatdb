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
database_engine = get_option('engine')

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
# Library build
#
lib_deps = [
  dependency('libudjat'),
]

lib_link_args = [ ]

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
                    '-fPIC'
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
app_conf.set_quoted('DATABASE_ENGINE', database_engine)

if cxx.compiles('#include <cppdb/defs.h>', name : 'cppdb')
  app_conf.set('HAVE_CPPDB', 1)
  lib_link_args += ['-lcppdb']
endif

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
sdk_conf = configuration_data()
sdk_conf.set('PACKAGE_NAME', meson.project_name())
sdk_conf.set('PACKAGE_VERSION', meson.project_version())
sdk_conf.set('PACKAGE_DESCRIPTION', project_description)
sdk_conf.set('prefix',get_option('prefix'))
sdk_conf.set('exec_prefix',get_option('prefix'))
sdk_conf.set('includedir',get_option('includedir'))
sdk_conf.set('libdir',get_option('libdir'))

if host_machine.system() == 'windows'
  sdk_conf.set('REQUIRES','')
  sdk_conf.set('DYNLIBS','-ludjatdb.dll')
  sdk_conf.set('STATICLIBS','-ludjatdb')
else
  sdk_conf.set('REQUIRES','')
  sdk_conf.set('DYNLIBS','-ludjatdb')
  sdk_conf.set('STATICLIBS','-l:libudjatdb.a')
endif

configure_file(
  input : 'sdk/dynamic.pc.in',
  output : meson.project_name() + '.pc',
  install: true,
  install_dir: get_option('libdir') + '/pkgconfig',
  configuration : sdk_conf
)

configure_file(
  input : 'sdk/static.pc.in',
  output : meson.project_name() + '-static.pc',
  install: true,
  install_dir: get_option('libdir') + '/pkgconfig',
  configuration : sdk_conf
)

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
  'src/library/statement.cc',
]

lib_src += [
  'src/library/engines/cppdb/alert.cc',
  'src/library/engines/cppdb/exec.cc',
]

# src/library/engines/sqlite/alert.cc
# src/library/engines/sqlite/exec.cc
# src/library/engines/sqlite/module.cc
# src/library/engines/sqlite/session.cc

module_src = [
  'src/module/init.cc',
]

test_src = [
  'src/testprogram/testprogram.cc',
]

#
# Targets
#
dynamic = shared_library(
  'udjat' + database_engine, 
  config_src + lib_src,
	install: true,
	version : libversion,
	gnu_symbol_visibility: 'hidden',
	link_args : lib_link_args,
	dependencies: lib_deps,
	include_directories: includes_dir
)

static = static_library(
  'udjat' + database_engine, 
  config_src + lib_src,
	install: true,
	dependencies: lib_deps,
	include_directories: includes_dir
)

module = shared_library(
  'udjat-module-database', 
  config_src + module_src,
	install: true,
	link_with : dynamic,
	gnu_symbol_visibility: 'hidden',
	dependencies: lib_deps,
	include_directories: includes_dir
)

executable(
	'dbtest',
	config_src + test_src,
  install: false,
	link_with : [ dynamic, module ],
	dependencies: lib_deps,
	include_directories: includes_dir
)



