#
# spec file for package mingw32-libipc3270
#
# Copyright (c) 2015 SUSE LINUX GmbH, Nuernberg, Germany.
# Copyright (C) <2008> <Banco do Brasil S.A.>
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

%define _libname libipc3270

%define __strip %{_mingw32_strip}
%define __objdump %{_mingw32_objdump}
%define _use_internal_dependency_generator 0
%define __find_requires %{_mingw32_findrequires}
%define __find_provides %{_mingw32_findprovides}
%define __os_install_post %{_mingw32_debug_install_post} \
                          %{_mingw32_install_post}
                          
#---[ Package header ]------------------------------------------------------------------------------------------------

Summary:		lib3270/pw3270 IPC client library for 32 bits Windows
Name:			mingw32-%{_libname}
Version:		5.3
Release:		0
License:		LGPL-3.0

Source:			%{_libname}-%{version}.tar.xz
URL:			https://github.com/PerryWerneck/%{_libname}

Group:			System/Libraries
BuildRoot:		/var/tmp/%{name}-%{version}

BuildRequires:	autoconf >= 2.61
BuildRequires:	automake
BuildRequires:	gettext-devel
BuildRequires:	xz
BuildRequires:	fdupes

BuildRequires:	mingw32-cross-binutils
BuildRequires:	mingw32-cross-gcc
BuildRequires:	mingw32-cross-gcc-c++
BuildRequires:	mingw32-cross-pkg-config
BuildRequires:	mingw32-filesystem
BuildRequires:	mingw32(lib:iconv)
BuildRequires:	mingw32(lib:intl)

BuildRequires:	mingw32(pkg:gtk+-win32-3.0)
BuildRequires:	mingw32(pkg:lib3270)
BuildRequires:	mingw32(pkg:libv3270)

%description
IPC client library for lib3270/pw3270.

Designed as framework for language bindings.

For more details, see https://softwarepublico.gov.br/social/pw3270/ .

#---[ Library ]-------------------------------------------------------------------------------------------------------

%define _product %(i686-w64-mingw32-pkg-config --variable=product_name lib3270)
%define MAJOR_VERSION %(echo %{version} | cut -d. -f1)
%define MINOR_VERSION %(echo %{version} | cut -d. -f2)
%define _libvrs %{MAJOR_VERSION}_%{MINOR_VERSION}

%package -n %{name}-%{_libvrs}
Summary:		TN3270 Access library
Group:			Development/Libraries/C and C++

Provides:		mingw32-%{_libname} = %{version}
Conflicts:		otherproviders(mingw32-%{_libname})

%description -n %{name}-%{_libvrs}
IPC client library for lib3270/pw3270.

Designed as framework for language bindings.

For more details, see https://softwarepublico.gov.br/social/pw3270/ .

#---[ Development ]---------------------------------------------------------------------------------------------------

%package devel

Summary:		TN3270 Access library development files
Group:			Development/Libraries/C and C++
Requires:		%{name}-%{_libvrs} = %{version}

%description devel
Header files for the ipc3270 library.

#---[ Plugin module for pw3270 main application ]----------------------------------------------------------------------

%package -n mingw32-%{_product}-plugin-ipc
Summary: IPC service plugin for %{_product}
Requires: mingw32-%{_product} >= 5.2

%description -n mingw32-%{_product}-plugin-ipc

PW3270 plugin exporting D-Bus objects to be used by the ipc3270 client library.

See more details at https://softwarepublico.gov.br/social/pw3270/

#---[ Build & Install ]-----------------------------------------------------------------------------------------------

%prep
%setup -n %{_libname}-%{version}

NOCONFIGURE=1 \
	./autogen.sh

%{_mingw32_configure}

%build
make all %{?_smp_mflags}

%{_mingw32_strip} \
	--strip-all \
	.bin/Release/*.dll


%install
%{_mingw32_makeinstall}
%_mingw32_find_lang libipc3270 langfiles

%fdupes %{buildroot}

%files -n %{name}-%{_libvrs} -f langfiles
%defattr(-,root,root)

%doc AUTHORS README.md
%license LICENSE

%dir %{_mingw32_datadir}/%{_product}

%{_mingw32_bindir}/*.dll

%files devel
%defattr(-,root,root)

%{_mingw32_libdir}/*.a

%{_mingw32_includedir}/lib3270/ipc.h
%{_mingw32_includedir}/lib3270/ipc

%{_mingw32_libdir}/pkgconfig/*.pc

%{_mingw32_datadir}/%{_product}/pot/*.pot

%dir %{_mingw32_datadir}/%{_product}/def
%{_mingw32_datadir}/%{_product}/def/*.def

%files -n mingw32-%{_product}-plugin-ipc
%defattr(-,root,root)

%dir %{_mingw32_libdir}/%{_product}-plugins
%{_mingw32_libdir}/%{_product}-plugins/ipcserver.dll

%changelog
