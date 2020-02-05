dnl
dnl Software pw3270, desenvolvido com base nos códigos fontes do  C3270  e  X3270
dnl (Paul Mattes Paul.Mattes@usa.net), de emulação de terminal 3270 para acesso a
dnl aplicativos mainframe. Registro no INPI sob o nome G3270.
dnl
dnl Copyright (C) <2008> <Banco do Brasil S.A.>
dnl
dnl Este programa é software livre. Você pode redistribuí-lo e/ou modificá-lo sob
dnl os termos da GPL v.2 - Licença Pública Geral  GNU,  conforme  publicado  pela
dnl Free Software Foundation.
dnl
dnl Este programa é distribuído na expectativa de  ser  útil,  mas  SEM  QUALQUER
dnl GARANTIA; sem mesmo a garantia implícita de COMERCIALIZAÇÃO ou  de  ADEQUAÇÃO
dnl A QUALQUER PROPÓSITO EM PARTICULAR. Consulte a Licença Pública Geral GNU para
dnl obter mais detalhes.
dnl
dnl Você deve ter recebido uma cópia da Licença Pública Geral GNU junto com este
dnl programa;  se  não, escreva para a Free Software Foundation, Inc., 59 Temple
dnl Place, Suite 330, Boston, MA, 02111-1307, USA
dnl
dnl Contatos:
dnl
dnl perry.werneck@gmail.com	(Alexandre Perry de Souza Werneck)
dnl erico.mendonca@gmail.com	(Erico Mascarenhas de Mendonça)
dnl

dnl Process this file with autoconf to produce a configure script.

dnl The minimum version of autoconf required.
AC_PREREQ(2.61)

dnl Initialise automake with the package name, version and
dnl bug-reporting address.
AC_INIT([ipc3270], [5.3], [perry.werneck@gmail.com])

dnl Place auxilliary scripts here.
AC_CONFIG_AUX_DIR([scripts])

dnl Compute the canonical host-system type
AC_CANONICAL_HOST

dnl Put macro definitions here (though they aren't used).
AC_CONFIG_HEADER([common/src/include/config.h])

dnl Initialise automake stuff.
AM_INIT_AUTOMAKE

dnl Check for iconv
AM_ICONV

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_SED
AC_PROG_LN_S

AC_LANG([C])

dnl ---------------------------------------------------------------------------
dnl Initialize defaults
dnl ---------------------------------------------------------------------------

CLIENT_LDFLAGS=""
CLIENT_LIBS=""
CLIENT_TARGETS="devel"

dnl ---------------------------------------------------------------------------
dnl Check for OS specifics
dnl ---------------------------------------------------------------------------

case "$host" in
	*-mingw32|*-pc-msys)
		app_cv_osname="windows"
		CFLAGS="$CFLAGS -D_WIN32_WINNT=0x0600"
		LIBS="$LIBS -lws2_32 -lwtsapi32 -lcomdlg32 -lkernel32"
		LDFLAGS="$LDFLAGS"
		CLIENT_LDFLAGS="-shared -static-libgcc -static-libstdc++ -Wl,--output-def,\$(@D)/\lib\$(MODULE_NAME).def"
		CLIENT_LIBS="-Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive,-Bdynamic"
		DLLEXT=".dll"

		CLIENT_TARGETS="windows-lib $CLIENT_TARGETS"

		app_win32_revision=$(date +%-y.%-m.%-d.%-H)
		AC_SUBST(WIN32_VERSION,$app_win32_revision)

		app_win32_file_version=$(date +%-y,%-m,%-d,%-H)
		AC_SUBST(WIN32_FILE_VERSION,$app_win32_file_version)

		AC_CONFIG_FILES(server/src/plugin/windows/resources.rc)
		AC_CONFIG_FILES(server/src/service/windows/resources.rc)
		AC_CONFIG_FILES(client/src/core/windows/resources.rc)

		app_cv_static='yes'

		;;

 	*)
		CFLAGS="$CFLAGS -pthread"
		CXXFLAGS="$CXXFLAGS -pthread"
		LDFLAGS="$LDFLAGS -pthread"
		app_cv_datadir="/usr/share"
		app_cv_confdir="/etc"
		app_cv_osname="linux"
		LOGDIR="/var/log"
		DLLEXT=".so"
		CLIENT_LDFLAGS="-shared"

		CLIENT_TARGETS="linux-lib $CLIENT_TARGETS"

		app_cv_static='no'

esac

AC_SUBST(DATADIR,$app_cv_datadir)
AC_SUBST(CONFDIR,$app_cv_confdir)
AC_SUBST(OSNAME,$app_cv_osname)
AC_SUBST(LIBS)
AC_SUBST(LOGDIR)
AC_SUBST(DLLEXT)
AC_SUBST(CLIENT_LDFLAGS)
AC_SUBST(CLIENT_LIBS)

AC_CHECK_FUNC(vasprintf, AC_DEFINE(HAVE_VASPRINTF) )

dnl ---------------------------------------------------------------------------
dnl Check for other programs
dnl ---------------------------------------------------------------------------

AC_PATH_TOOL([AR], [ar], [ar])
AC_PATH_TOOL([WINDRES], [windres], [no])
AC_PATH_TOOL([DLLTOOL], [dlltool], [no])

PKG_CHECK_EXISTS

dnl ---------------------------------------------------------------------------
dnl Version info
dnl ---------------------------------------------------------------------------

AC_SUBST(PACKAGE_DESCRIPTION,"PW3270 IPC Module")

app_vrs_major=$(echo $VERSION | cut -d. -f1)
app_vrs_minor=$(echo $VERSION | cut -d. -f2)

AC_DEFINE_UNQUOTED(PACKAGE_MAJOR_VERSION, $app_vrs_major)
AC_DEFINE_UNQUOTED(PACKAGE_MINOR_VERSION, $app_vrs_minor)

AC_SUBST(PACKAGE_MAJOR_VERSION,$app_vrs_major)
AC_SUBST(PACKAGE_MINOR_VERSION,$app_vrs_minor)

AC_ARG_WITH([release], [AS_HELP_STRING([--with-release], [Set release])], [ app_cv_release="$withval" ],[ app_cv_release="`date +%-y`.`date +%-m`.`date +%-d`" ])

app_rls_major=$(echo $app_cv_release.0.0 | cut -d. -f1)
app_rls_minor=$(echo $app_cv_release.0.0 | cut -d. -f2)

AC_DEFINE_UNQUOTED(PACKAGE_RELEASE, $app_cv_release)
AC_DEFINE_UNQUOTED(PACKAGE_MAJOR_RELEASE, $app_rls_major)
AC_DEFINE_UNQUOTED(PACKAGE_MINOR_RELEASE, $app_rls_minor)

AC_SUBST(PACKAGE_RELEASE,$app_cv_release)
AC_SUBST(PACKAGE_MAJOR_RELEASE,$app_rls_major)
AC_SUBST(PACKAGE_MINOR_RELEASE,$app_rls_minor)

case "$host" in
	*-mingw32|*-pc-msys)
		AC_SUBST(LIBEXT,.dll)
		;;

 	s390x-*)
		AC_SUBST(LIBEXT,.so)
		;;

 	*)
		AC_SUBST(LIBEXT,.so.$app_vrs_major.$app_vrs_minor)

esac

dnl ---------------------------------------------------------------------------
dnl Build options
dnl ---------------------------------------------------------------------------

AC_ARG_ENABLE([static],
 	[AS_HELP_STRING([--enable-static], [Enable build and install the static library])],
[
 	app_cv_static="$enableval"
],[
	AC_MSG_NOTICE([Keep default selection for static library.])
])

if test "$app_cv_static" == "yes"; then
	CLIENT_TARGETS="$CLIENT_TARGETS static"
fi

AC_SUBST(CLIENT_TARGETS)

dnl ---------------------------------------------------------------------------
dnl Check for libintl
dnl ---------------------------------------------------------------------------

INTL_LIBS=""

AC_PATH_TOOL([XGETTEXT], [xgettext], [no])
AC_PATH_TOOL([MSGCAT], [msgcat], [no])
AC_PATH_TOOL([MSGINIT], [msginit], [no])
AC_PATH_TOOL([MSGMERGE], [msgmerge], [no])
AC_PATH_TOOL([MSGFMT], [msgfmt], [no])
AC_PATH_TOOL([VALGRIND], [valgrind], [no])

dnl ---------------------------------------------------------------------------
dnl Check for LIB3270
dnl ---------------------------------------------------------------------------

AC_ARG_WITH([delayed-lib3270],
        [AS_HELP_STRING([--with-delayed-lib3270], [Uses delayed version of lib3270 (WIN32 Only)])],
[
	app_cv_delayed_lib3270=$withval
],[

	case "$host" in
	*-mingw32|*-pc-msys)
		app_cv_delayed_lib3270="yes"
		;;

	*)
		app_cv_delayed_lib3270="no"

	esac

])

if test "$app_cv_delayed_lib3270" == "yes"; then

	PKG_CHECK_MODULES( [LIB3270], [lib3270-delayed], AC_DEFINE(LIB3270_DELAYED), AC_MSG_ERROR([LIB3270 delayed module is not present.]))
	AC_MSG_NOTICE([Not using libintl to avoid the dependency.])

else
	PKG_CHECK_MODULES( [LIB3270], [lib3270], AC_DEFINE(LIB3270_DYNAMIC), AC_MSG_ERROR([LIB3270 dynamic module is not present.]))

	# Use libintl only if using dynamic lib3270 to avoid an extra DLL dependency.
	AC_CHECK_HEADER(libintl.h, [

		AC_DEFINE(HAVE_LIBINTL, 1)

		case "$host" in
		*-mingw32|*-pc-msys)
			INTL_LIBS="-lintl"
			;;

		*)
			INTL_LIBS=""

		esac

	])

fi

AC_SUBST(INTL_LIBS)


AC_SUBST(LIB3270_LIBS)
AC_SUBST(LIB3270_CFLAGS)

AC_ARG_WITH([product-name], [AS_HELP_STRING([--with-product-name], [Set product name])], [ app_cv_product="$withval" ],[ app_cv_product=`pkg-config --variable=product_name lib3270` ])
AC_DEFINE_UNQUOTED(PRODUCT_NAME, $app_cv_product)
AC_SUBST(PRODUCT_NAME,$app_cv_product)

dnl ---------------------------------------------------------------------------
dnl Check for LIBV3270
dnl ---------------------------------------------------------------------------

PKG_CHECK_MODULES( [V3270], [libv3270], AC_DEFINE(HAVE_V3270), AC_MSG_ERROR([V3270 not present.]))

AC_SUBST(V3270_LIBS)
AC_SUBST(V3270_CFLAGS)

dnl ---------------------------------------------------------------------------
dnl Check for GTK
dnl ---------------------------------------------------------------------------

PKG_CHECK_MODULES( [GTK], [gtk+-3.0], AC_DEFINE(HAVE_GTK), AC_MSG_ERROR([GTK not present.]))

AC_SUBST(GTK_LIBS)
AC_SUBST(GTK_CFLAGS)

dnl ---------------------------------------------------------------------------
dnl Check for GLIB
dnl ---------------------------------------------------------------------------

PKG_CHECK_MODULES( [GLIB], [glib-2.0 gobject-2.0 gmodule-2.0 gio-2.0], AC_DEFINE(HAVE_GLIB), AC_MSG_ERROR([GLIB not present.] ))
AC_SUBST(GLIB_LIBS)
AC_SUBST(GLIB_CFLAGS)

dnl ---------------------------------------------------------------------------
dnl Check for D-Bus
dnl ---------------------------------------------------------------------------

PKG_CHECK_MODULES( [DBUS], [dbus-1], app_cv_libdbus=yes, AC_MSG_NOTICE([ No DBUS support.]) )
AC_SUBST(DBUS_LIBS)
AC_SUBST(DBUS_CFLAGS)

PKG_CHECK_MODULES( [GDBUS], [dbus-glib-1], app_cv_dbus=yes, AC_MSG_NOTICE([No GDBUS support.])  )
AC_SUBST(GDBUS_LIBS)
AC_SUBST(GDBUS_CFLAGS)

dnl ---------------------------------------------------------------------------
dnl systemd integration
dnl ---------------------------------------------------------------------------

AC_ARG_ENABLE([systemd],
	AS_HELP_STRING([--enable-systemd], [Use systemd]),
	[with_systemd=$enableval],
	[with_systemd=auto])

PKG_CHECK_MODULES(systemd,
	[systemd],
	[have_systemd=yes], [have_systemd=no])

if test x$with_systemd = xauto ; then
        if test x$have_systemd = xno ; then
                with_systemd=no
        else
                with_systemd=yes
        fi
fi

if (test "${with_systemd}" = "yes"); then

    PKG_CHECK_MODULES(SYSTEMD, systemd, dummy=yes,AC_MSG_ERROR(systemd is required))

    SYSTEMD_UNITDIR="`$PKG_CONFIG --variable=systemdsystemunitdir systemd`"
	AC_SUBST(SYSTEMD_UNITDIR)

	PKG_CHECK_MODULES( [SYSTEMD], [libsystemd], AC_DEFINE(HAVE_SYSTEMD), AC_MSG_NOTICE([libsystemd not present.]) )

	AC_SUBST(SYSTEMD_CFLAGS)
	AC_SUBST(SYSTEMD_LIBS)

	AC_CONFIG_FILES(conf/systemd.service)

	INIT_MODULE="systemd"

else

	INIT_MODULE="initd"

fi

AC_SUBST(INIT_MODULE)

dnl ---------------------------------------------------------------------------
dnl Check for doxygen
dnl ---------------------------------------------------------------------------

AC_PATH_TOOL([DOXYGEN], [doxygen])

dnl Help line for graphviz (optionally used by doxygen)
AC_ARG_ENABLE(graphviz,
	AS_HELP_STRING([--disable-graphviz], [prevent doxygen from using dot]),
	[wantgraphviz=$enableval], [wantgraphviz=yes])

AC_CHECK_PROGS([GRAPHVIZDOT], [dot])
AC_MSG_CHECKING([whether doxygen should use graphviz])
if test "x$wantgraphviz" = "xyes" ; then
	if test $GRAPHVIZDOT ; then
		AC_MSG_RESULT([yes])
		usegraphviz=yes
	else
		AC_MSG_RESULT([no, graphviz missing])
		usegraphviz=no
	fi
else
	AC_MSG_RESULT([no, disabled by configure])
	usegraphviz=no
fi

AC_SUBST(HAVE_DOT,`echo $usegraphviz | tr yesno YESNO`)

if test "x$DOXYGEN" != "x"; then
	AC_CONFIG_FILES(doxygen/doxyfile)
fi

dnl ---------------------------------------------------------------------------
dnl Check for pic
dnl ---------------------------------------------------------------------------
AC_ARG_ENABLE([pic],
	[AS_HELP_STRING([--disable-pic], [disable support for PIC libraries (required to compile on 64bits)])],
[
	app_cv_pic="$enableval"
],[

	case "$host_cpu" in
	x86_64)
		app_cv_pic="yes"
		;;

	aarch64)
		app_cv_pic="yes"
		;;

	*)
		app_cv_pic="no"
	esac

])

if test "$app_cv_pic" == "yes"; then
	CFLAGS="$CFLAGS -fPIC"
	CXXFLAGS="$CXXFLAGS -fPIC"
fi

dnl ---------------------------------------------------------------------------
dnl Check for C++ 2011 support
dnl ---------------------------------------------------------------------------
AC_DEFUN([AX_CHECK_COMPILE_FLAG],
[AC_PREREQ(2.59)dnl for _AC_LANG_PREFIX
AS_VAR_PUSHDEF([CACHEVAR],[ax_cv_check_[]_AC_LANG_ABBREV[]flags_$4_$1])dnl
AC_CACHE_CHECK([whether _AC_LANG compiler accepts $1], CACHEVAR, [
  ax_check_save_flags=$[]_AC_LANG_PREFIX[]FLAGS
  _AC_LANG_PREFIX[]FLAGS="$[]_AC_LANG_PREFIX[]FLAGS $4 $1"
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
    [AS_VAR_SET(CACHEVAR,[yes])],
    [AS_VAR_SET(CACHEVAR,[no])])
  _AC_LANG_PREFIX[]FLAGS=$ax_check_save_flags])
AS_IF([test x"AS_VAR_GET(CACHEVAR)" = xyes],
  [m4_default([$2], :)],
  [m4_default([$3], :)])
AS_VAR_POPDEF([CACHEVAR])dnl
])dnl AX_CHECK_COMPILE_FLAGS

AX_CHECK_COMPILE_FLAG([-std=gnu++11], [CXXFLAGS="$CXXFLAGS -std=gnu++11"])

dnl ---------------------------------------------------------------------------
dnl Set translation domain.
dnl ---------------------------------------------------------------------------

app_cv_gettext_package="libipc3270"

AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$app_cv_gettext_package")
AC_SUBST(GETTEXT_PACKAGE,"$app_cv_gettext_package")

dnl ---------------------------------------------------------------------------
dnl Configure which files to generate.
dnl ---------------------------------------------------------------------------

AC_CONFIG_FILES(Makefile)
AC_CONFIG_FILES(server/Makefile)
AC_CONFIG_FILES(client/Makefile)
AC_CONFIG_FILES(locale/Makefile)
AC_CONFIG_FILES(client/sdk/ipc3270.pc)
AC_CONFIG_FILES(client/sdk/ipc3270-static.pc)

dnl ---------------------------------------------------------------------------
dnl Output the generated config.status script.
dnl ---------------------------------------------------------------------------
AC_SUBST(BASEDIR,$ac_pwd)
CFLAGS="$CFLAGS -DHAVE_CONFIG_H"
CXXFLAGS="$CXXFLAGS -DHAVE_CONFIG_H"

AC_OUTPUT


