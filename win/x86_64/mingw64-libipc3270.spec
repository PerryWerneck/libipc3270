#
# spec file for packages mingw64-libipc3270
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

%define __strip %{_mingw64_strip}
%define __objdump %{_mingw64_objdump}
%define _use_internal_dependency_generator 0
%define __find_requires %{_mingw64_findrequires}
%define __find_provides %{_mingw64_findprovides}
%define __os_install_post %{_mingw64_debug_install_post} \
                          %{_mingw64_install_post}

#---[ Main package ]--------------------------------------------------------------------------------------------------

Name:		mingw64-libipc3270
Summary:	lib3270/pw3270 IPC client library.
Version:	5.2
Release:	0
License:	LGPL-3.0
Source:		libipc3270-%{version}.tar.xz

Url:		https://github.com/PerryWerneck/libipc3270.git

Group:		Development/Libraries/C and C++

BuildRequires:  autoconf >= 2.61
BuildRequires:  automake
BuildRequires:  binutils
BuildRequires:  coreutils
BuildRequires:  gcc-c++
BuildRequires:  gettext-devel
BuildRequires:  m4

BuildRequires:	mingw64-cross-binutils
BuildRequires:	mingw64-cross-gcc
BuildRequires:	mingw64-cross-gcc-c++
BuildRequires:	mingw64-cross-pkg-config
BuildRequires:	mingw64-filesystem
BuildRequires:	mingw64-zlib-devel
BuildRequires:	mingw64(lib:iconv)
BuildRequires:	mingw64(lib:intl)

BuildRequires:	mingw64(pkg:gtk+-win32-3.0)
BuildRequires:	mingw64(pkg:lib3270)
BuildRequires:	mingw64(pkg:libv3270)

%description

IPC client library for lib3270/pw3270.

Designed as framework for language bindings.

See more details at https://softwarepublico.gov.br/social/pw3270/

#---[ Library ]-------------------------------------------------------------------------------------------------------

%define _product %(x86_64-w64-mingw32-pkg-config --variable=product_name lib3270)
%define MAJOR_VERSION %(echo %{version} | cut -d. -f1)
%define MINOR_VERSION %(echo %{version} | cut -d. -f2)
%define _libvrs %{MAJOR_VERSION}_%{MINOR_VERSION}

%package -n %{name}-%{_libvrs}
Summary:	IPC Library for pw3270
Group:		Development/Libraries/C and C++
Provides:	mingw64(lib:ipc3270)


%description -n %{name}-%{_libvrs}

IPC client library for lib3270/pw3270.

Designed as framework for language bindings.

See more details at https://softwarepublico.gov.br/social/pw3270/

#---[ Development ]---------------------------------------------------------------------------------------------------

%package devel
Summary: Development files for %{name}
Requires: mingw64(pkg:lib3270)

%description devel

Development files for lib3270/pw3270 IPC client library.

Designed as a support tool for language bindings.

#---[ Plugin module for pw3270 main application ]----------------------------------------------------------------------

%package -n mingw64-%{_product}-plugin-ipc
Summary: IPC service plugin for %{_product}
Requires: mingw64-%{_product} >= 5.2

%description -n mingw64-%{_product}-plugin-ipc

PW3270 plugin exporting D-Bus objects for every tn3270 session.

See more details at https://softwarepublico.gov.br/social/pw3270/

#---[ Build & Install ]-----------------------------------------------------------------------------------------------

%prep
%setup -n libipc3270-%{version}

NOCONFIGURE=1 \
	./autogen.sh

%{_mingw64_configure} \
	--enable-static

%build
make all

%{_mingw64_strip} \
	--strip-all \
    .bin/Release/*.dll

%install
rm -rf $RPM_BUILD_ROOT

%makeinstall

%files -n %{name}-%{_libvrs}
%defattr(-,root,root)

%doc AUTHORS README.md
%license LICENSE

%{_mingw64_libdir}/libipc3270.dll
%{_mingw64_libdir}/libipc3270.dll.%{MAJOR_VERSION}
%{_mingw64_libdir}/libipc3270.dll.%{MAJOR_VERSION}.%{MINOR_VERSION}

%files -n mingw64-%{_product}-plugin-ipc
%defattr(-,root,root)

%dir %{_mingw64_libdir}/%{_product}-plugins
%{_mingw64_libdir}/%{_product}-plugins/ipcserver.dll

%files devel
%defattr(-,root,root)
%{_mingw64_includedir}/lib3270/ipc.h
%dir %{_mingw64_includedir}/lib3270/ipc
%{_mingw64_includedir}/lib3270/ipc/*.h
%{_mingw64_libdir}/pkgconfig/*.pc
%{_mingw64_libdir}/*.a
%{_mingw64_libdir}/*.lib

%{_mingw64_datadir}/%{_product}/def/*.def

%changelog

