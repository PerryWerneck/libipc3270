/*
 * "Software pw3270, desenvolvido com base nos códigos fontes do WC3270  e X3270
 * (Paul Mattes Paul.Mattes@usa.net), de emulação de terminal 3270 para acesso a
 * aplicativos mainframe. Registro no INPI sob o nome G3270.
 *
 * Copyright (C) <2008> <Banco do Brasil S.A.>
 *
 * Este programa é software livre. Você pode redistribuí-lo e/ou modificá-lo sob
 * os termos da GPL v.2 - Licença Pública Geral  GNU,  conforme  publicado  pela
 * Free Software Foundation.
 *
 * Este programa é distribuído na expectativa de  ser  útil,  mas  SEM  QUALQUER
 * GARANTIA; sem mesmo a garantia implícita de COMERCIALIZAÇÃO ou  de  ADEQUAÇÃO
 * A QUALQUER PROPÓSITO EM PARTICULAR. Consulte a Licença Pública Geral GNU para
 * obter mais detalhes.
 *
 * Você deve ter recebido uma cópia da Licença Pública Geral GNU junto com este
 * programa; se não, escreva para a Free Software Foundation, Inc., 51 Franklin
 * St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Este programa está nomeado como - e possui - linhas de código.
 *
 * Contatos:
 *
 * perry.werneck@gmail.com	(Alexandre Perry de Souza Werneck)
 * erico.mendonca@gmail.com	(Erico Mascarenhas Mendonça)
 *
 */

/**
 * @file ipc-client-internals.h
 *
 * @brief Private definitions for lib3270/pw3270 ipc client library.
 *
 * @author perry.werneck@gmail.com
 *
 */

#ifndef IPC_CLIENT_INTERNALS_INCLUDED

	#define IPC_CLIENT_INTERNALS_INCLUDED

	#ifdef HAVE_CONFIG_H
        #include <config.h>
	#endif // HAVE_CONFIG_H

	#ifdef _WIN32
		#include <winsock2.h>
		#include <windows.h>
		#include <ws2tcpip.h>
	#else
		#include <dbus/dbus.h>
	#endif // WIN32

	#include <iostream>
	#include <mutex>
	#include <lib3270/ipc.h>
	#include <lib3270/ipc/request.h>
	#include <lib3270/popup.h>
	#include <system_error>
	#include <stdexcept>
	#include <string>

#ifndef HAVE_ICONV
	#include <lib3270/charset.h>
#endif // !HAVE_ICONV

#ifdef HAVE_LIBINTL
	#include <libintl.h>
	#define _( x ) dgettext(GETTEXT_PACKAGE, x)
	#define N_( x )	x
#else
	#define _( x ) x
	#define N_( x ) x
#endif // HAVE_LIBINTL

//
// Compiler-specific #defines.
//
// Reference: GLIBC gmacros.h
//
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
        #define GNUC_UNUSED \
                __attribute__((__unused__))
#else
        #define GNUC_UNUSED
#endif

#if defined(_MSC_VER)
	// MSVC doesn't have a strcasecmp() function; instead it has _stricmp.
	#define strcasecmp(a,b) _stricmp(a,b)
#endif // _MSC_VER

#ifdef HAVE_ICONV
	#include <iconv.h>
#endif // HAVE_ICONV

#ifdef DEBUG

	inline void console(std::ostream &out) {
		out << std::endl;
	}

	template<typename T, typename... Targs>
	void console(std::ostream &out, T value, Targs... Fargs) {
		out << value;
		console(out, Fargs...);
	}

	template<typename T, typename... Targs>
	void log(T value, Targs... Fargs) {
		console(std::clog,value,Fargs...);
	}

	#define debug(...) log(__FILE__, "(", __LINE__, ") ", __VA_ARGS__);

#else

	#define debug(...) /* __VA_ARGS__ */

#endif

	namespace TN3270 {

		/// @brief Check lib3270 return codes, launch exception when failed.
		TN3270_PRIVATE void chkResponse(int rc);

#ifdef _WIN32

		TN3270_PRIVATE std::string getUserName();

#endif // _WIN32


	}

#endif // IPC_CLIENT_INTERNALS_INCLUDED
