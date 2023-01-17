dnl SPDX-License-Identifier: LGPL-3.0-or-later
dnl
dnl Copyright (C) 2008 Banco do Brasil S.A.
dnl
dnl This program is free software: you can redistribute it and/or modify
dnl it under the terms of the GNU Lesser General Public License as published
dnl by the Free Software Foundation, either version 3 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU Lesser General Public License
dnl along with this program.  If not, see <https://www.gnu.org/licenses/>.
dnl

dnl Process this file with autoconf to produce a configure script.

dnl The minimum version of autoconf required.
AC_PREREQ(2.59)

dnl Initialise automake with the package name, version and
dnl bug-reporting address.
AC_INIT([libipc3270], [5.4], [perry.werneck@gmail.com], [libipc3270], [https://github.com/PerryWerneck/libipc3270])

dnl Place auxilliary scripts here.
AC_CONFIG_AUX_DIR([scripts])

dnl Compute the canonical host-system type
AC_CANONICAL_HOST

dnl Put macro definitions here (though they aren't used).
AC_CONFIG_HEADER([src/include/config.h])

dnl Initialise automake stuff.
AM_INIT_AUTOMAKE

dnl Check for iconv
AM_ICONV

dnl Set gettext version
AM_GNU_GETTEXT_VERSION([0.14])

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_SED
AC_PROG_LN_S

AC_LANG([C++])

dnl ---------------------------------------------------------------------------
dnl Version info - https://semver.org/
dnl ---------------------------------------------------------------------------

app_vrs_major=$(echo $VERSION | cut -d. -f1)
app_vrs_minor=$(echo $VERSION | cut -d. -f2)
app_vrs_patch=0

AC_DEFINE_UNQUOTED(PACKAGE_MAJOR_VERSION, $app_vrs_major, [The library major version])
AC_DEFINE_UNQUOTED(PACKAGE_MINOR_VERSION, $app_vrs_minor, [The library minor version])
AC_DEFINE_UNQUOTED(PACKAGE_PATCH_LEVEL, $app_vrs_patch, [The patch level])

AC_SUBST(PACKAGE_MAJOR_VERSION,$app_vrs_major)
AC_SUBST(PACKAGE_MINOR_VERSION,$app_vrs_minor)
AC_SUBST(PACKAGE_PATCH_LEVEL,$app_vrs_patch)

AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"libudjat-$app_vrs_major.$app_vrs_minor",[The gettext package name.])
AC_SUBST(GETTEXT_PACKAGE,"libipc3270-$app_vrs_major.$app_vrs_minor")

dnl ---------------------------------------------------------------------------
dnl Set application id & product name
dnl ---------------------------------------------------------------------------

AC_ARG_WITH([product-name], [AS_HELP_STRING([--with-product-name], [Set product name])], [ app_cv_product="$withval" ],[ app_cv_product=`pkg-config --variable=product_name lib3270` ])
AC_DEFINE_UNQUOTED(PRODUCT_NAME, "$app_cv_product", [The product name])
AC_SUBST(PRODUCT_NAME,$app_cv_product)

AC_ARG_WITH([application-id], [AS_HELP_STRING([--application-id], [Set the application ID])], [ app_cv_application_id="$withval" ],[ app_cv_application_id=`pkg-config --variable=product_id lib3270`.terminal ])

AC_DEFINE_UNQUOTED(APPLICATION_ID, "$app_cv_application_id", [The application ID])
AC_SUBST(APPLICATION_ID,$app_cv_application_id)

app_cv_dbus_path="/"`echo $app_cv_application_id | sed -e "s@\.@/@g"`
AC_DEFINE_UNQUOTED(DBUS_OBJECT_PATH, "$app_cv_dbus_path",[The D-Bus object path])
AC_SUBST(DBUS_OBJECT_PATH,$app_cv_dbus_path)


dnl ---------------------------------------------------------------------------
dnl Set translation domain.
dnl ---------------------------------------------------------------------------

app_cv_gettext_package="libipc3270-$app_vrs_major.$app_vrs_minor"

AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$app_cv_gettext_package",[The Gettext package name])
AC_SUBST(GETTEXT_PACKAGE,"$app_cv_gettext_package")

dnl ---------------------------------------------------------------------------
dnl Check for PKG-Config
dnl ---------------------------------------------------------------------------

PKG_CHECK_EXISTS

dnl ---------------------------------------------------------------------------
dnl Check for OS specifics
dnl ---------------------------------------------------------------------------

CFLAGS="$CFLAGS -I$ac_pwd/src/include"
CXXFLAGS="$CXXFLAGS -I$ac_pwd/src/include"

case "$host" in
	*-mingw32|*-pc-msys)
		CFLAGS="$CFLAGS -pthread"
		#  -D_WIN32_WINNT=0x0600
		LDFLAGS="$LDFLAGS -pthread -Wl,--output-def,\$(@D)/\$(PACKAGE_NAME).def"
		LIBS="-lws2_32 -lshlwapi $LIBS -lshell32 -lole32 -luuid -liphlpapi"		
		AC_SUBST(LIBEXT,.dll)
		AC_SUBST(EXEEXT,.exe)
		AC_SUBST(SONAME,libipc3270.dll)
		AC_SUBST(OSNAME,windows)
		;;

 	*)
		CFLAGS="$CFLAGS -pthread -DLIBDIR=\"\$(libdir)\" -DDATADIR=\"\$(datarootdir)\""
		CXXFLAGS="$CXXFLAGS -DLIBDIR=\"\$(libdir)\" -DDATADIR=\"\$(datarootdir)\""
		LDFLAGS="$LDFLAGS -pthread"
		LIBS="$LIBS -ldl"
		AC_SUBST(LIBEXT,.so)
		AC_SUBST(EXEEXT)
		AC_SUBST(SONAME,libipc3270.so.$app_vrs_major.$app_vrs_minor)
		AC_SUBST(OSNAME,linux)

		PKG_CHECK_MODULES( [DBUS], [dbus-1], app_cv_libdbus=yes, AC_MSG_ERROR([ No DBUS support.]) )
		AC_SUBST(DBUS_LIBS)
		AC_SUBST(DBUS_CFLAGS)

		PKG_CHECK_MODULES( [GDBUS], [dbus-glib-1], app_cv_dbus=yes, AC_MSG_ERROR([No GDBUS support.])  )
		AC_SUBST(GDBUS_LIBS)
		AC_SUBST(GDBUS_CFLAGS)

esac

dnl ---------------------------------------------------------------------------
dnl Check for tools
dnl ---------------------------------------------------------------------------

AC_PATH_TOOL([VALGRIND], [valgrind], [no])
AC_PATH_TOOL([AR], [ar], [ar])
AC_PATH_TOOL([DLLTOOL], [dlltool], [true])
AC_PATH_TOOL([STRIP], [strip], [true])
AC_PATH_TOOL([STRIP], [strip], [true])
AC_PATH_TOOL([WINDRES], [windres], [true])
AC_PATH_TOOL([DOXYGEN], [doxygen], [true])

dnl ---------------------------------------------------------------------------
dnl Check for libintl
dnl ---------------------------------------------------------------------------

AC_PATH_TOOL([XGETTEXT], [xgettext], [true])
AC_PATH_TOOL([MSGCAT], [msgcat], [true])
AC_PATH_TOOL([MSGINIT], [msginit], [true])
AC_PATH_TOOL([MSGMERGE], [msgmerge], [true])
AC_PATH_TOOL([MSGFMT], [msgfmt], [true])

dnl ---------------------------------------------------------------------------
dnl Check for C++ 2017 support
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

AX_CHECK_COMPILE_FLAG([-std=c++17], [CPPFLAGS="$CPPFLAGS -std=c++17"], [AC_MSG_ERROR(["C++ 17 is required"])])

dnl ---------------------------------------------------------------------------
dnl Set the ELF symbol visibility support
dnl ---------------------------------------------------------------------------
AC_DEFINE(HAVE_GNUC_VISIBILITY, 1, [supports GCC visibility attributes])

dnl ---------------------------------------------------------------------------
dnl Check for compiler visibility flag
dnl ---------------------------------------------------------------------------
AC_MSG_CHECKING(whether $CC accepts -fvisibility=hidden)
AC_CACHE_VAL(app_cv_fvisibility_ok,
[ac_save_cc="$CC"
CC="$CC -fvisibility=hidden"
AC_TRY_RUN([int main() { return 0; }],
  app_cv_fvisibility_ok=yes,
  app_cv_fvisibility_ok=no,
  app_cv_fvisibility_ok=no)
CC="$ac_save_cc"])
AC_MSG_RESULT($app_cv_fvisibility_ok)
if test $app_cv_fvisibility_ok = yes; then
  CFLAGS="$CFLAGS -fvisibility=hidden"
  CXXFLAGS="$CXXFLAGS -fvisibility=hidden"
fi

dnl ---------------------------------------------------------------------------
dnl Check for pic
dnl ---------------------------------------------------------------------------
AC_ARG_ENABLE([pic],
	[AS_HELP_STRING([--disable-pic], [disable support for PIC libraries (required to compile on 64bits)])],
[
	app_cv_pic="$enableval"
],[

	case "$host" in
		*-mingw32|*-pc-msys)
			app_cv_pic="no"
		;;

 	s390x-*)
			app_cv_pic="yes"
		;;

	*)
		app_cv_pic="yes"
	esac
])

if test "$app_cv_pic" == "yes"; then
	CFLAGS="$CFLAGS -fPIC"
	CXXFLAGS="$CXXFLAGS -fPIC"
fi

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

	PKG_CHECK_MODULES( [LIB3270], [lib3270-delayed], AC_DEFINE(LIB3270_DELAYED,1,[Using lib3270 delayed]), AC_MSG_ERROR([LIB3270 delayed module is not present.]))
	AC_MSG_NOTICE([Not using libintl to avoid the dependency.])

else
	PKG_CHECK_MODULES( [LIB3270], [lib3270], AC_DEFINE(LIB3270_DYNAMIC,1,[Using standard lib3270]), AC_MSG_ERROR([LIB3270 dynamic module is not present.]))

	# Use libintl only if using dynamic lib3270 to avoid an extra DLL dependency.
	AC_CHECK_HEADER(libintl.h, [

		AC_DEFINE(HAVE_LIBINTL, 1,[Using libintl])

		case "$host" in
		*-mingw32|*-pc-msys)
			INTL_LIBS="-lintl"
			;;

		*)
			INTL_LIBS=""

		esac

	])

fi

AC_SUBST(LIB3270_LIBS)
AC_SUBST(LIB3270_CFLAGS)

dnl ---------------------------------------------------------------------------
dnl Configure which files to generate.
dnl ---------------------------------------------------------------------------

AC_CONFIG_FILES(sdk/dynamic.pc)
AC_CONFIG_FILES(sdk/static.pc)
AC_CONFIG_FILES(locale/Makefile)
AC_CONFIG_FILES(Makefile)

dnl ---------------------------------------------------------------------------
dnl Output the generated config.status script.
dnl ---------------------------------------------------------------------------

AC_OUTPUT

