#
# spec file for packages mingw64-pw3270-plugin-ipc
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
%define _product %(x86_64-w64-mingw32-pkg-config --variable=product_name lib3270)


%define __strip %{_mingw64_strip}
%define __objdump %{_mingw64_objdump}
%define _use_internal_dependency_generator 0
%define __find_requires %{_mingw64_findrequires}
%define __find_provides %{_mingw64_findprovides}
%define __os_install_post %{_mingw64_debug_install_post} \
                          %{_mingw64_install_post}

#---[ Main package ]--------------------------------------------------------------------------------------------------

Summary:	D-Bus based IPC plugin for pw3270
Name:		mingw64-pw3270-plugin-ipc
Version:	5.2
Release:	0
License:	LGPL-3.0
Source:		pw3270-plugin-ipc-%{version}.tar.xz

Url:		https://portal.softwarepublico.gov.br/social/pw3270/

Group:		System/X11/Terminals
BuildRoot:	/var/tmp/%{name}-%{version}

Provides:	mingw64-lib3270-ipc-service
Conflicts:	otherproviders(mingw64-lib3270-ipc-service)

BuildRequires:	mingw64-lib3270-devel >= 5.2
BuildRequires:	mingw64-libv3270-devel >= 5.2
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

PW3270 IPC plugin.

See more details at https://softwarepublico.gov.br/social/pw3270/

#---[ IPC Library Package ]-------------------------------------------------------------------------------------------

%package -n mingw64-libipc3270-%{MAJOR_VERSION}_%{MINOR_VERSION}
Summary: IPC Library for pw3270

Recommends: mingw64-lib3270-ipc-service
Provides:	mingw64(lib:ipc3270)

%description -n mingw64-libipc3270-%{MAJOR_VERSION}_%{MINOR_VERSION}

IPC client library for lib3270/pw3270.

Designed as a support tool for language bindings.

%package -n mingw64-libipc3270-devel
Summary: Development files for ipc3270
Requires: libipc3270-%{MAJOR_VERSION}_%{MINOR_VERSION}
Requires: pkgconfig(lib3270)

%description -n mingw64-libipc3270-devel

Development files for lib3270/pw3270 IPC client library.

Designed as a support tool for language bindings.


#---[ Build & Install ]-----------------------------------------------------------------------------------------------

%prep
%setup -n pw3270-plugin-ipc-%{version}

NOCONFIGURE=1 ./autogen.sh

%{_mingw64_configure}

%build
make clean
make all

%{_mingw64_strip} \
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

%dir %{_mingw64_libdir}/%{_product}-plugins
%{_mingw64_libdir}/%{_product}-plugins/ipcserver.dll

%files -n mingw64-libipc3270-%{MAJOR_VERSION}_%{MINOR_VERSION}
%defattr(-,root,root)
%{_mingw64_libdir}/libipc3270.dll
%{_mingw64_libdir}/libipc3270.dll.%{MAJOR_VERSION}
%{_mingw64_libdir}/libipc3270.dll.%{MAJOR_VERSION}.%{MINOR_VERSION}

%files -n mingw64-libipc3270-devel
%defattr(-,root,root)
%{_mingw64_includedir}/lib3270/ipc.h
%dir %{_mingw64_includedir}/lib3270/ipc
%{_mingw64_includedir}/lib3270/ipc/*.h
%{_mingw64_libdir}/*.a
%{_mingw64_libdir}/pkgconfig/*.pc

%changelog

