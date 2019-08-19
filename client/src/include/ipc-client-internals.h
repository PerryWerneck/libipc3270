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

	#include <config.h>

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
	#include <lib3270/popup.h>
	#include <system_error>
	#include <stdexcept>

#ifdef HAVE_LIBINTL
        #include <libintl.h>
        #define _( x )                  gettext(x)
        #define N_( x )                 x
#else
        #define _( x )                  x
        #define N_( x )                 x
#endif // HAVE_LIBINTL

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
#endif

				/// @brief Converte charset.
				static std::string convertCharset(iconv_t &converter, const char *str, int length);

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
				virtual std::string	get(int baddr, size_t len, char lf) const = 0;
				virtual std::string	get(int row, int col, size_t sz, char lf) const = 0;

				// Set strings to lib3270 without charset conversion.
				virtual void set(const std::string &str) = 0;
				virtual void set(int baddr, const std::string &str) = 0;
				virtual void set(int row, int col, const std::string &str) = 0;

			public:

				// Contents
				std::string	toString(int baddr = 0, size_t len = -1, char lf = '\n') const override;
				std::string	toString(int row, int col, size_t sz, char lf = '\n') const override;

				void push(const char *text, size_t length) override;
				void push(int baddr, const char *text, int length) override;
				void push(int row, int col, const char *text, int length) override;

				void pop(int baddr, std::string &text) override;
				void pop(int row, int col, std::string &text) override;
				void pop(std::string &text) override;

			};

		}

		/// @brief lib3270 direct access objects (no IPC);
		namespace Local {

			TN3270_PRIVATE Session * getSessionInstance();

		}

		/// @brief IPC Based acess (Access an active instance of pw3270 or pw3270d)
		namespace IPC {

			TN3270_PRIVATE TN3270::Session * getSessionInstance(const char *id);

			class Session;

			/// @brief PW3270 IPC Request/Response.
			class Request {
			private:
				Request(const IPC::Session &session);

#ifdef _WIN32
				/// @brief Pipe Handle.
				HANDLE hPipe;

				/// @brief IPC Data type.
				enum Type : uint8_t {
					String	= 's',
					Boolean	= 'b',
					Uchar	= 'y',
					Int16	= 'n',
					Uint16	= 'q',
					Int32	= 'i',
					Int32x	= 'h',
					Uint32	= 'u',
					Int64	= 'x',
					Uint64	= 't'
				};

				struct {
					DWORD 	  length;	///< @brief Length of input buffer.
					DWORD	  used;		///< @brief Length of used block.
					DWORD	  current;	///< @brief Offset of the current argument.
					uint8_t * block;
				} in;

				struct {
					DWORD 	  length;
					DWORD	  used;
					uint8_t * block;
				} out;

				struct DataBlock {
					Type type;
				};

				/// @brief Store value on data block.
				DataBlock * pushBlock(const void *ptr, size_t len);

				/// @brief Get next argument.
				DataBlock * getNextBlock() const;

#else
				struct {
					DBusMessage		* in;
					DBusMessage		* out;
					DBusMessageIter	  iter;

				} msg;
				DBusConnection	* conn;

#endif // _WIN32

			public:

				/// @brief Create a method call.
				Request(const IPC::Session &session, const char *method);

				/// @brief Create a get/set property call.
				///
				/// @param session	Session object.
				/// @param isSet	true if this is a setProperty call.
				/// @param property	Property name.
				//
				Request(const IPC::Session &session, bool isSet, const char *property);

				~Request();

				Request & call();

				// Push values
				Request & push(const char *arg);
				Request & push(const bool arg);
				Request & push(const int32_t arg);
				Request & push(const uint32_t arg);
				Request & push(const uint8_t arg);

				// Pop values
				Request & pop(std::string &value);
				Request & pop(int &value);
				Request & pop(unsigned int &value);

			};

		}

	}

#endif // IPC_CLIENT_INTERNALS_INCLUDED
