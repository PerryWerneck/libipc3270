#
# spec file for packages mingw32-pw3270-plugin-ipc
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

#---[ Versions ]------------------------------------------------------------------------------------------------------

%define MAJOR_VERSION 5
%define MINOR_VERSION 2

%define _libvrs %{MAJOR_VERSION}_%{MINOR_VERSION}

%define __strip %{_mingw32_strip}
%define __objdump %{_mingw32_objdump}
%define _use_internal_dependency_generator 0
%define __find_requires %{_mingw32_findrequires}
%define __find_provides %{_mingw32_findprovides}
%define __os_install_post %{_mingw32_debug_install_post} \
                          %{_mingw32_install_post}

#---[ Main package ]--------------------------------------------------------------------------------------------------

Summary:	D-Bus based IPC plugin for pw3270
Name:		mingw32-pw3270-plugin-ipc
Version:	5.2
Release:	0
License:	LGPL-3.0
Source:		pw3270-plugin-ipc-%{version}.tar.xz

Url:		https://portal.softwarepublico.gov.br/social/pw3270/

Group:		System/X11/Terminals
BuildRoot:	/var/tmp/%{name}-%{version}

Provides:	mingw32-lib3270-ipc-service
Conflicts:	otherproviders(mingw32-lib3270-ipc-service)

BuildRequires:	mingw32-lib3270-devel >= 5.2
BuildRequires:	mingw32-libv3270-devel >= 5.2
BuildRequires:  autoconf >= 2.61
BuildRequires:  automake
BuildRequires:  binutils
BuildRequires:  coreutils
BuildRequires:  gcc-c++
BuildRequires:  gettext-devel
BuildRequires:  m4

BuildRequires:	mingw32-cross-binutils
BuildRequires:	mingw32-cross-gcc
BuildRequires:	mingw32-cross-gcc-c++
BuildRequires:	mingw32-cross-pkg-config
BuildRequires:	mingw32-filesystem
BuildRequires:	mingw32-zlib-devel
BuildRequires:	mingw32(lib:iconv)
BuildRequires:	mingw32(lib:intl)

BuildRequires:	mingw32(pkg:gtk+-win32-3.0)
BuildRequires:	mingw32(pkg:lib3270)
BuildRequires:	mingw32(pkg:lib3270-static)
BuildRequires:	mingw32(pkg:libv3270)

%description

PW3270 IPC plugin.

See more details at https://softwarepublico.gov.br/social/pw3270/

#---[ IPC Library Package ]-------------------------------------------------------------------------------------------

%package -n mingw32-libipc3270-%{MAJOR_VERSION}_%{MINOR_VERSION}
Summary: IPC Library for pw3270

Recommends: mingw32-lib3270-ipc-service
Provides:	mingw32(lib:ipc3270)

%description -n mingw32-libipc3270-%{MAJOR_VERSION}_%{MINOR_VERSION}

IPC client library for lib3270/pw3270.

Designed as a support tool for language bindings.

%package -n mingw32-libipc3270-devel
Summary: Development files for ipc3270
Requires: libipc3270-%{MAJOR_VERSION}_%{MINOR_VERSION}
Requires: pkgconfig(lib3270)

%description -n mingw32-libipc3270-devel

Development files for lib3270/pw3270 IPC client library.

Designed as a support tool for language bindings.


#---[ Build & Install ]-----------------------------------------------------------------------------------------------

%prep
%setup -n pw3270-plugin-ipc-%{version}

NOCONFIGURE=1 ./autogen.sh

%{_mingw32_configure} \
	--without-static-lib3270

%build
make clean
make all

%{_mingw32_strip} \
	--strip-all \
    .bin/Release/*.dll

%install
rm -rf $RPM_BUILD_ROOT

%makeinstall

%files
%defattr(-,root,root)

# https://en.opensuse.org/openSUSE:Packaging_for_Leap#RPM_Distro_Version_Macros
%if 0%{?sle_version} > 120200
%doc AUTHORS README.md
%license LICENSE
%else
%doc AUTHORS README.md LICENSE
%endif

%dir %{_mingw32_libdir}/pw3270-plugins
%{_mingw32_libdir}/pw3270-plugins/ipcserver.dll

%files -n mingw32-libipc3270-%{MAJOR_VERSION}_%{MINOR_VERSION}
%defattr(-,root,root)
%{_mingw32_libdir}/libipc3270.dll
%{_mingw32_libdir}/libipc3270.dll.%{MAJOR_VERSION}
%{_mingw32_libdir}/libipc3270.dll.%{MAJOR_VERSION}.%{MINOR_VERSION}

%files -n mingw32-libipc3270-devel
%defattr(-,root,root)
%{_mingw32_includedir}/lib3270/ipc.h
%{_mingw32_libdir}/*.a
%{_mingw32_libdir}/pkgconfig/*.pc

%changelog

