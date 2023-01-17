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
AC_INIT([ipc3270], [5.4], [perry.werneck@gmail.com])

dnl Place auxilliary scripts here.
AC_CONFIG_AUX_DIR([scripts])

dnl Compute the canonical host-system type
AC_CANONICAL_HOST

dnl Initialise automake stuff.
AM_INIT_AUTOMAKE

dnl Check for iconv
AM_ICONV

dnl Set gettext version
AM_GNU_GETTEXT_VERSION([0.14])

dnl Checks for programs.
AC_PROG_SED
AC_PROG_LN_S

AC_LANG([C])

dnl ---------------------------------------------------------------------------
dnl Configure which files to generate.
dnl ---------------------------------------------------------------------------

dnl AC_CONFIG_SUBDIRS([sample])
dnl https://www.gnu.org/software/autoconf-archive/ax_subdirs_configure.html
dnl AX_SUBDIRS_CONFIGURE( [subdirs], [mandatory arguments], [possibly merged arguments], [replacement arguments], [forbidden arguments])
dnl AX_SUBDIRS_CONFIGURE([sample],[UDJAT_CFLAGS=-I$ac_pwd/src/include],[UDJAT_LIBS=-L$ac_pwd/.bin/Debug/libudjat.so.$app_vrs_major.$app_vrs_minor],[--with-build-dir=$ac_pwd])

dnl ac_configure_args="UDJAT_CFLAGS=-I$ac_pwd/src/include UDJAT_LIBS=-L$ac_pwd/.bin/Debug/libudjat.so.$app_vrs_major.$app_vrs_minor --with-build-dir=$ac_pwd"

dnl https://www.gnu.org/software/autoconf-archive/ax_subdirs_configure.html
AX_SUBDIRS_CONFIGURE([plugin])
AX_SUBDIRS_CONFIGURE([client])

AC_CONFIG_FILES(Makefile)

AC_OUTPUT


