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

		namespace Abstract {

			class TN3270_PRIVATE Session : public TN3270::Session {
			private:

#ifdef HAVE_ICONV
				struct {

					/// @brief Convert strings from host codepage to local codepage.
					iconv_t local;

					/// @brief Convert string from local codepage to host codepage.
					iconv_t	host;

				} converter;

				static std::string convertCharset(iconv_t &converter, const char *str, int length);

#else

				LIB3270_ICONV * converter;

#endif

				/// @brief Converte charset.

			protected:

				Session();
				virtual ~Session();

				/// @brief Setup charsets
				void setCharSet(const char *remote, const char *local);

				/// @brief Converte string recebida do host para o charset atual.
				std::string convertFromHost(const std::string &str) const;

				/// @brief Converte string do charset atual para o charset do host.
				std::string convertToHost(const std::string &str) const;
				std::string convertToHost(const char *text, int length) const;

				// Get strings from lib3270 without charset conversion.
				virtual std::string	get() const = 0;
				virtual std::string	get(int baddr, int len, char lf) const = 0;
				virtual std::string	get(unsigned int row, unsigned int col, int sz, char lf) const = 0;

				// Set strings to lib3270 without charset conversion.
				virtual void set(const std::string &str) = 0;
				virtual void set(int baddr, const std::string &str) = 0;
				virtual void set(int row, int col, const std::string &str) = 0;

			public:

				// Contents
				std::string	toString(int baddr, int len, char lf) const override;
				std::string	toString(unsigned short row, unsigned short col, int len, char lf) const override;

				void push(const char *text, size_t length) override;
				void push(int baddr, const char *text, int length) override;
				void push(unsigned short row, unsigned short col, const char *text, int length) override;

				void pop(int baddr, std::string &text) override;
				void pop(unsigned short row, unsigned short col, std::string &text) override;
				void pop(std::string &text) override;

			};

		}

		/// @brief lib3270 direct access objects (no IPC);
		namespace Local {

			TN3270_PRIVATE Session * getSessionInstance(const char *charset = nullptr);

		}

		/// @brief IPC Based acess (Access an active instance of pw3270 or pw3270d)
		namespace IPC {

			TN3270_PRIVATE TN3270::Session * getSessionInstance(const char *id, const char *charset = nullptr);
			class Session;

		}

	}

#endif // IPC_CLIENT_INTERNALS_INCLUDED
