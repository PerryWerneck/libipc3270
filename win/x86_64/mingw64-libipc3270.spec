#
# spec file for package mingw64-libipc3270
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

%define __strip %{_mingw64_strip}
%define __objdump %{_mingw64_objdump}
%define _use_internal_dependency_generator 0
%define __find_requires %{_mingw64_findrequires}
%define __find_provides %{_mingw64_findprovides}
%define __os_install_post %{_mingw64_debug_install_post} \
                          %{_mingw64_install_post}
                          
#---[ Package header ]------------------------------------------------------------------------------------------------

Summary:		lib3270/pw3270 IPC client library for 64 bits Windows
Name:			mingw64-%{_libname}
Version:		5.2
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
BuildRequires:	libtool

BuildRequires:	mingw64-cross-binutils
BuildRequires:	mingw64-cross-gcc
BuildRequires:	mingw64-cross-gcc-c++
BuildRequires:	mingw64-cross-pkg-config
BuildRequires:	mingw64-filesystem
BuildRequires:	mingw64(lib:iconv)
BuildRequires:	mingw64(lib:intl)

BuildRequires:	mingw64(pkg:gtk+-win32-3.0)
BuildRequires:	mingw64(lib:3270.delayed)
BuildRequires:	mingw64(pkg:libv3270)

%define _product %(x86_64-w64-mingw32-pkg-config --variable=product_name lib3270)

%define MAJOR_VERSION %(echo %{version} | cut -d. -f1)
%define MINOR_VERSION %(echo %{version} | cut -d. -f2)
%define _libvrs %{MAJOR_VERSION}_%{MINOR_VERSION}

%description
IPC client library for lib3270/%{_product}.
Designed as framework for language bindings.

%package -n %{name}-%{_libvrs}
Summary:		TN3270 Access library
Group:			Development/Libraries/C and C++

%description -n %{name}-%{_libvrs}
IPC client library for lib3270/%{_product}.
Designed as framework for language bindings.

%package devel

Summary:		TN3270 Access library development files
Group:			Development/Libraries/C and C++
Requires:		%{name}-%{_libvrs} = %{version}

%description devel
Header files for %{name}.

%prep
%setup -n %{_libname}-%{version}

NOCONFIGURE=1 \
	./autogen.sh

%{_mingw64_configure}

%build
make all %{?_smp_mflags}

%{_mingw64_strip} \
	--strip-all \
	.bin/Release/*.dll


%install
%{_mingw64_makeinstall}
%_mingw64_find_lang %{_libname}-%{MAJOR_VERSION}.%{MINOR_VERSION} langfiles
%fdupes %{buildroot}

%files -n %{name}-%{_libvrs} -f langfiles
%defattr(-,root,root)

%doc AUTHORS README.md
%license LICENSE

%dir %{_mingw64_datadir}/%{_product}

%{_mingw64_bindir}/*.dll

%files devel
%defattr(-,root,root)

%{_mingw64_libdir}/*.a

%{_mingw64_includedir}/*/ipc.h
%{_mingw64_includedir}/*/ipc

%{_mingw64_libdir}/pkgconfig/*.pc

%dir %{_mingw64_datadir}/*/def
%{_mingw64_datadir}/*/def/*.def

%changelog

