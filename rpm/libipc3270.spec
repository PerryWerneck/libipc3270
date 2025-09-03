#
# spec file for package libipc3270
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

# Please submit bugfixes or comments via https://github.com/PerryWerneck/libipc3270/issues
#

%define product %(pkg-config --variable=product_name lib3270)

Summary:		IPC client library for lib3270/%{product} 
Name:			libipc3270
Version: 5.5.0
Release:		0
License:		LGPL-3.0
Source:			%{name}-%{version}.tar.xz

URL:			https://github.com/PerryWerneck/libipc3270

Group:			Development/Libraries/C and C++
BuildRoot:		/var/tmp/%{name}-%{version}

BuildRequires:	fdupes
BuildRequires:	gettext-devel
BuildRequires:	meson >= 0.56.0
BuildRequires:	pkgconfig
BuildRequires:	gcc-c++
BuildRequires:	gettext-devel
BuildRequires:	gettext-tools

%if 0%{?fedora} ||  0%{?suse_version} > 1200

BuildRequires:	pkgconfig(lib3270) >= 5.4
BuildRequires:	pkgconfig(dbus-1)
BuildRequires:	pkgconfig(dbus-glib-1)

%else

BuildRequires:	lib3270-devel >= 5.4
BuildRequires:	dbus-1-devel
BuildRequires:	dbus-glib-devel

%endif

%description
IPC client library for lib3270/%{product}.

Designed as framework for language bindings.

For more details, see https://github.com/PerryWerneck/libipc3270 .

#---[ Library ]-------------------------------------------------------------------------------------------------------

%define MAJOR_VERSION %(echo %{version} | cut -d. -f1)
%define MINOR_VERSION %(echo %{version} | cut -d. -f2 | cut -d+ -f1)
%define _libvrs %{MAJOR_VERSION}_%{MINOR_VERSION}

%package -n %{name}-%{_libvrs}
Summary: IPC Library for %{product}

%description -n %{name}-%{_libvrs}
IPC client library for lib3270/%{product}.

Designed as framework for language bindings.

For more details, see https://github.com/PerryWerneck/libipc3270 .

#---[ Development ]---------------------------------------------------------------------------------------------------

%package devel
Summary: Development files for %{name}
Requires: %{name}-%{_libvrs} = %{version}

%if 0%{?fedora} ||  0%{?suse_version} > 1200
Requires:	pkgconfig(lib3270) >= 5.4
%else
Requires:	lib3270-devel >= 5.4
%endif

%description devel

Development files for lib3270/%{product} IPC client library.

For more details, see https://github.com/PerryWerneck/libipc3270 .

%lang_package -n %{name}-%{_libvrs}

#---[ Build & Install ]-----------------------------------------------------------------------------------------------

%prep
%autosetup
%meson

%build
%meson_build

%install
%meson_install

%find_lang %{name}-%{MAJOR_VERSION}.%{MINOR_VERSION} langfiles

%files -n %{name}-%{_libvrs}
%defattr(-,root,root)
%{_libdir}/%{name}.so.*.*

%files devel
%defattr(-,root,root)
%{_includedir}/lib3270/ipc.h
%dir %{_includedir}/lib3270/ipc
%{_includedir}/lib3270/ipc/*.h
%{_libdir}/%{name}.so
%{_libdir}/%{name}.a
%{_libdir}/pkgconfig/*.pc

%files -n %{name}-%{_libvrs}-lang -f langfiles

%pre -n %{name}-%{_libvrs} -p /sbin/ldconfig

%post -n %{name}-%{_libvrs} -p /sbin/ldconfig

%postun -n %{name}-%{_libvrs} -p /sbin/ldconfig

%changelog

