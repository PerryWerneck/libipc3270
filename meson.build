# Maintainer: Perry Werneck <perry.werneck@gmail.com>

# SPDX-License-Identifier: LGPL-3.0-or-later 

# Copyright (C) 2025 Banco do Brasil S. A.
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
	'ipc3270', 
	['cpp'],
	version: '5.5.0',
	license: 'GPL-3.0-or-later',
)

project_description = 'IPC library for pw3270'

#
# Compiler flags
#
cc = meson.get_compiler('cpp')

enable_debug = get_option('debug') or get_option('buildtype').contains('debug')

time_epoch = run_command('sh', '-c', 'echo "$SOURCE_DATE_EPOCH"', check : true).stdout().strip()
if time_epoch == ''
  time_epoch = run_command('date','+%Y%m%d', check: true).stdout().strip()
  datecmd = 'date'
else
  datecmd = 'date -u -d@' + time_epoch
endif

add_project_arguments(cc.get_supported_arguments([
	'-ggdb3',
	'-fvisibility=hidden',
	'-DHAVE_CONFIG_H=1',
	'-DBUILD_DATE='+time_epoch,
	get_option('buildtype').contains('debug') ? '-DDEBUG=1' : '-DNDEBUG=1',
]), language: 'cpp')

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
subdir('po')

#
# Dependencies
#
libintl = dependency('intl', required: false)
threads = dependency('threads')
libiconv = dependency('iconv', required : false)
dbus = dependency('dbus-1', required: false)

if cc.get_id() == 'msvc'

	lib3270 = dependency(
		'lib3270-delayed'
	)

else 

	lib3270 = dependency(
		'lib3270', 
		fallback: [ 'lib3270', 'static_library' ]
	)

endif

#
# Configuration
#
product_id = lib3270.get_variable('product_id')
app_conf = configuration_data()
app_conf.set_quoted('PACKAGE_NAME', meson.project_name())
app_conf.set('PRODUCT_NAME', lib3270.get_variable('product_name'))
app_conf.set('PRODUCT_ID', product_id)
app_conf.set_quoted('PACKAGE_VERSION', meson.project_version())
app_conf.set_quoted('PACKAGE_DESCRIPTION', project_description)
app_conf.set('PACKAGE_VERSION_MAJOR', pkg_major_version)
app_conf.set('PACKAGE_VERSION_MINOR', pkg_minor_version)
app_conf.set('PACKAGE_VERSION_MICRO', pkg_micro_version)

# FIXME: use product_id from lib3270
dbus_object_path= '/' + product_id.replace('.','/')
app_conf.set_quoted('DBUS_OBJECT_PATH',dbus_object_path)

app_conf.set('HAVE_LIBINTL', 1)
app_conf.set('HAVE_GNUC_VISIBILITY', 1)

if cc.compiles('#include <unistd.h>', name : 'unistd.h')
	app_conf.set('HAVE_UNISTD_H', 1)
endif

#
# Sources
#
includes_dir = include_directories(
    'src/include',
)

lib_src = [
	'src/core/actions.cc',
	'src/core/constants.cc',  
	'src/core/events.cc',
	'src/core/property.cc',
	'src/core/request.cc',
	'src/core/session.cc',
	'src/core/versions.cc',
	'src/host/actions.cc',
	'src/host/pop.cc',
	'src/host/push.cc',
	'src/host/stream.cc',
	'src/host/init.cc',
	'src/host/properties.cc',
	'src/host/states.cc',
	'src/host/string.cc',
	'src/session/action.cc',
	'src/session/get.cc',
	'src/session/set.cc',
	'src/session/tools.cc',
	'src/session/local/actions.cc',    
	'src/session/local/events.cc',
	'src/session/local/get.cc',
	'src/session/local/properties.cc',
	'src/session/local/wait.cc',
	'src/session/local/attribute.cc',
	'src/session/local/field.cc',
	'src/session/local/init.cc',
	'src/session/local/set.cc',
	'src/session/remote/actions.cc',
	'src/session/remote/get.cc',
	'src/session/remote/session.cc',
	'src/session/remote/wait.cc',
	'src/session/remote/attribute.cc',
	'src/session/remote/properties.cc',
	'src/session/remote/set.cc',
	'src/session/remote/field.cc',
	'src/session/remote/request.cc',
	'src/session/remote/tools.cc',
	'src/core/' + host_machine.system() + '/request.cc',
	'src/session/remote/' + host_machine.system() + '/request.cc',
	'src/session/remote/' + host_machine.system() + '/session.cc',
]

#
# SDK
# https://mesonbuild.com/Pkgconfig-module.html
#
pkg = import('pkgconfig')

pkg_variables = [
	'product_name=@0@'.format(lib3270.get_variable('product_name')),
	'product_id=@0@'.format(product_id),
	'sdk_version=@0@'.format(lib3270.get_variable('sdk_version')),
	'version_major=@0@'.format(pkg_major_version),
	'version_minor=@0@'.format(pkg_minor_version),
	'version_micro=@0@'.format(pkg_micro_version),
	'dbus_object_path=@0@'.format(dbus_object_path),
	'module_version=@0@.@1@'.format(pkg_major_version,pkg_minor_version),
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

if host_machine.system() == 'windows'

	pkg.generate(
		name: 'lib' + meson.project_name(),
		description: project_description,
		variables: pkg_variables,
		requires_private: [ 'lib3270' ],
		libraries: [ '-lipc3270.dll' ]
	)

	# https://mesonbuild.com/Windows-module.html
	windows = import('windows')
	resources = windows.compile_resources(
		configure_file(
			input : 'src/core/windows/resources.rc.in',
			output : 'resources.rc',
			configuration : app_conf
		)
	)

	dynamic = shared_library(
		meson.project_name(), 
		lib_src + resources,
		install: true,
		gnu_symbol_visibility: 'hidden',
		dependencies: [
			libintl,
			libiconv,
			threads,
			lib3270,
		],
		link_args: [
			'-lshlwapi', 
			'-lshell32', 
			'-lole32', 
			'-luuid', 
			'-liphlpapi',
			'-lws2_32',
		],
		include_directories: includes_dir,
	)

	static_library = declare_dependency(
		link_with : static_library(
		meson.project_name(), 
			lib_src,
			pic: true,
			c_args: [ '-DSTATIC_LIBRARY' ],
			install: true,
			include_directories: includes_dir,
		dependencies: [
			libintl,
			libiconv,
			threads,
			lib3270,
		],
		),
		link_args: [
			'-lshlwapi', 
			'-lshell32', 
			'-lole32', 
			'-luuid', 
			'-liphlpapi',
			'-lws2_32',
		],
		include_directories : includes_dir,
		variables: pkg_variables,
	)

elif host_machine.system() == 'darwin'

	pkg.generate(
		name: 'lib' + meson.project_name(),
		description: project_description,
		variables: pkg_variables,
		requires: [ 'lib3270' ],
		libraries: [ '-lipc3270' ],
	)

	dynamic = shared_library(
		meson.project_name(), 
		lib_src,
		install: true,
		version : libversion,
		soversion : libversion,
		c_args: [ 
			'-fPIC' 
		],
		gnu_symbol_visibility: 'hidden',
		dependencies: [
			libintl,
			libiconv,
			threads,
			lib3270,
		],
		include_directories: includes_dir
	)

	static_library = declare_dependency(
		link_with : static_library(
			meson.project_name(), 
			lib_src,
			pic: true,
			c_args: [ '-DSTATIC_LIBRARY' ],
			install: true,
			include_directories: includes_dir,
			dependencies: [
				libintl,
				libiconv,
				threads,
				lib3270,
			],
		),
		include_directories : includes_dir,
		variables: pkg_variables,
	)

else

  pkg.generate(
    name: 'lib' + meson.project_name(),
    description: project_description,
    variables: pkg_variables,
	requires: [ 'lib3270' ],
	requires_private: [ dbus ],
	libraries: [ '-lipc3270' ],
  )

  dynamic = shared_library(
	meson.project_name(), 
    lib_src,
    install: true,
    version : libversion,
    cpp_args: [ '-ffat-lto-objects' ],
    soversion : libversion,
    gnu_symbol_visibility: 'hidden',
	dependencies: [
		libintl,
		dbus,
		libiconv,
		threads,
		lib3270,
	],
    include_directories: includes_dir
  )

  static_library = declare_dependency(
	link_with : static_library(
		meson.project_name(), 
	    lib_src,
		pic: true,
		cpp_args: [ '-DSTATIC_LIBRARY', '-fno-lto' ],
		install: true,
		include_directories: includes_dir,
		dependencies: [
			libintl,
			dbus,
			libiconv,
			threads,
			lib3270,
		],
	),
	include_directories : includes_dir,
	variables: pkg_variables,
  )
 
endif

dynamic_library = declare_dependency(
	link_with : dynamic,
	include_directories : includes_dir,
	variables: pkg_variables,
) 

executable(
	meson.project_name(),
	[ 
		'src/testprogram/testprogram.cc',
	],
	install: false,
	link_with : [ dynamic ],
	dependencies: [
		libintl,
		libiconv,
		threads,
		lib3270,
	],
	include_directories: includes_dir
)

install_headers(
  'src/include/lib3270/ipc.h',
  subdir: 'lib3270'  
)

install_headers(
	'src/include/lib3270/ipc/field.h',
	'src/include/lib3270/ipc/host.h',
	'src/include/lib3270/ipc/property.h',
	'src/include/lib3270/ipc/request.h',
	'src/include/lib3270/ipc/session.h',
	'src/include/lib3270/ipc/action.h',
  subdir: 'lib3270/ipc'  
)
