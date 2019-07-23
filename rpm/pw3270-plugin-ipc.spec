#
# spec file for packages pw3270-plugin-ipc
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

#---[ Macros ]--------------------------------------------------------------------------------------------------------

%if ! %{defined _release}
  %define _release suse%{suse_version}
%endif

#---[ Main package ]--------------------------------------------------------------------------------------------------

Summary:	D-Bus based IPC plugin for pw3270
Name:		pw3270-plugin-ipc
Version:	5.2
Release:	0
License:	LGPL-3.0
Source:		%{name}-%{version}.tar.xz

Url:		https://portal.softwarepublico.gov.br/social/pw3270/

Group:		System/X11/Terminals
BuildRoot:	/var/tmp/%{name}-%{version}

Provides:	pw3270-plugin-dbus
Conflicts:	otherproviders(pw3270-plugin-dbus)

Provides:	lib3270-ipc-service

Requires:	pw3270 >= 5.2

BuildRequires:	lib3270-devel >= 5.2
BuildRequires:	libv3270-devel >= 5.2
BuildRequires:  autoconf >= 2.61
BuildRequires:  automake
BuildRequires:  binutils
BuildRequires:  coreutils
BuildRequires:  gcc-c++
BuildRequires:  gettext-devel
BuildRequires:  m4

%if 0%{?fedora} ||  0%{?suse_version} > 1200

BuildRequires:  pkgconfig(openssl)
BuildRequires:  pkgconfig(dbus-1)
BuildRequires:  pkgconfig(dbus-glib-1)
BuildRequires:	pkgconfig(gtk+-3.0)

%else

BuildRequires:  openssl-devel
BuildRequires:  dbus-1-devel
BuildRequires:  dbus-glib-devel
BuildRequires:	gtk3-devel

%endif

%if 0%{?centos_version}
# centos requires python for genmarshal
BuildRequires:  python
%endif

%description

PW3270 plugin exporting D-Bus objects for every tn3270 session.

See more details at https://softwarepublico.gov.br/social/pw3270/

#---[ IPC Library Package ]-------------------------------------------------------------------------------------------

%package -n libipc3270-%{MAJOR_VERSION}_%{MINOR_VERSION}
Summary: IPC Library for pw3270
Recommends: lib3270-ipc-service

%description -n libipc3270-%{MAJOR_VERSION}_%{MINOR_VERSION}

IPC client library for lib3270/pw3270.

Designed as a support tool for language bindings.

%package -n libipc3270-devel
Summary: Development files for ipc3270
Requires: libipc3270-%{MAJOR_VERSION}_%{MINOR_VERSION}

%description -n libipc3270-devel

Development files for lib3270/pw3270 IPC client library.

Designed as a support tool for language bindings.


#---[ Build & Install ]-----------------------------------------------------------------------------------------------

%prep
%setup

NOCONFIGURE=1 ./autogen.sh

%configure

%build
make clean
make all

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

%dir %{_libdir}/pw3270-plugins
%{_libdir}/pw3270-plugins/ipcserver.so

%files -n libipc3270-%{MAJOR_VERSION}_%{MINOR_VERSION}
%defattr(-,root,root)
%{_libdir}/libipc3270.so.%{MAJOR_VERSION}.%{MINOR_VERSION}
%{_libdir}/libipc3270.so.%{MAJOR_VERSION}

%files -n libipc3270-devel
%defattr(-,root,root)
%{_includedir}/lib3270/ipc.h
%{_libdir}/libipc3270.so

%pre -n libipc3270-%{MAJOR_VERSION}_%{MINOR_VERSION}
/sbin/ldconfig
exit 0

%post -n libipc3270-%{MAJOR_VERSION}_%{MINOR_VERSION}
/sbin/ldconfig
exit 0

%postun -n libipc3270-%{MAJOR_VERSION}_%{MINOR_VERSION}
/sbin/ldconfig
exit 0

%changelog

