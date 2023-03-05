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
 * Este programa está nomeado como ipc.h e possui - linhas de código.
 *
 * Contatos:
 *
 * perry.werneck@gmail.com	(Alexandre Perry de Souza Werneck)
 * erico.mendonca@gmail.com	(Erico Mascarenhas Mendonça)
 *
 */

#ifndef LIB3270_IPC_H_INCLUDED

	#define LIB3270_IPC_H_INCLUDED 1

	#include <iostream>
	#include <cstdarg>
	#include <vector>
	#include <functional>
	#include <lib3270.h>
	#include <lib3270/keyboard.h>
	#include <lib3270/actions.h>
	#include <lib3270/ssl.h>

	#if defined(_WIN32) || defined(_MSC_VER)

		#define TN3270_PUBLIC	__declspec (dllexport)
		#define TN3270_PRIVATE

	#elif defined(__SUNPRO_C) && (__SUNPRO_C >= 0x550)

		#define TN3270_PUBLIC
		#define TN3270_PRIVATE

	#elif defined(__GNUC__)

		#define TN3270_PUBLIC	__attribute__((visibility("default")))
		#define TN3270_PRIVATE	__attribute__((visibility("hidden")))

	#else

		#error Unable to set visibility attribute

	#endif

#ifdef __cplusplus

	#include <string>

	namespace TN3270 {

		class Host;
		class Controller;
		class Action;
		class Session;
		class Field;

		#define DEFAULT_TIMEOUT 5

		/**
		 * @brief Get IPC module library version.
		 *
		 * @return Constant string with the IPC module library version (It's not the same of lib3270).
		 *
		 */
		TN3270_PUBLIC const char * getVersion() noexcept;

		/**
		 * @brief Get IPC module library revision.
		 *
		 * @return Constant string with the IPC module library revision (It's not the same of lib3270).
		 *
		 */
		TN3270_PUBLIC const char * getRevision() noexcept;

		/**
		 * @brief Get protocol library install location.
		 *
		 * @return The installation path of the tn3270 protocol library.
		 *
		 */
		TN3270_PUBLIC std::string getInstallLocation();

		/**
		 * @brief Get list of attributes.
		 *
		 */
		TN3270_PUBLIC bool for_each(const std::function<bool(const LIB3270_PROPERTY &property)> &method);

		/**
		 * @brief Get list of actions.
		 *
		 */
		TN3270_PUBLIC bool for_each(const std::function<bool(const LIB3270_ACTION &action)> &method);

		/**
		 * @brief Lib3270 dynamic memory block wrapper.
		 *
		 */
		template<typename T>
		class lib3270_ptr {
		private:
			T *ptr;

		public:
			lib3270_ptr(T *d) : ptr(d) {
			}

			~lib3270_ptr() {
				lib3270_free((void *) this->ptr);
			}

			operator bool() const noexcept {
				return this->ptr != NULL;
			}

			T * operator->() {
				return this->ptr;
			}

			operator T *() const noexcept {
				return this->ptr;
			}

		};

		/**
		 * @brief TN3270 Event.
		 *
		 */
		class TN3270_PUBLIC Event {
		public:

			/// @brief Event type.
			enum Type : uint8_t {
				All,			///< @brief All events (undefined).
				Popup,			///< @brief Popup message.
				Trace,			///< @brief Trace message.
				Message,		///< @brief Generic message.
				Connection		///< @brief Connect/Disconnect event.
			};

		private:
			Type type;

		protected:

			/// @brief Creates an event.
			///
			/// @param Type of the event.
			Event(enum Type type);

		public:
			virtual ~Event();

			/// @brief Check event type
			inline bool is(Event::Type type) const noexcept {
				return this->type == type;
			}

			/// @brief Check event type
			inline bool operator==(Event::Type type) const noexcept {
				return this->type == type;
			}

			inline operator Event::Type() const noexcept {
				return this->type;
			}

			/// @brief Get event description.
			virtual std::string toString() const = 0;

		};

		/// @brief TN3270 Program message.
		enum ProgramMessage : uint8_t {
			MESSAGE_NONE			= LIB3270_MESSAGE_NONE,				///< @brief No message
			MESSAGE_SYSWAIT			= LIB3270_MESSAGE_SYSWAIT,			///< @brief --
			MESSAGE_TWAIT			= LIB3270_MESSAGE_TWAIT,			///< @brief --
			MESSAGE_CONNECTED		= LIB3270_MESSAGE_CONNECTED,		///< @brief Connected
			MESSAGE_DISCONNECTED	= LIB3270_MESSAGE_DISCONNECTED,		///< @brief Disconnected from host
			MESSAGE_AWAITING_FIRST	= LIB3270_MESSAGE_AWAITING_FIRST,	///< @brief --
			MESSAGE_MINUS			= LIB3270_MESSAGE_MINUS,			///< @brief --
			MESSAGE_PROTECTED		= LIB3270_MESSAGE_PROTECTED,		///< @brief --
			MESSAGE_NUMERIC			= LIB3270_MESSAGE_NUMERIC,			///< @brief --
			MESSAGE_OVERFLOW		= LIB3270_MESSAGE_OVERFLOW,			///< @brief --
			MESSAGE_INHIBIT			= LIB3270_MESSAGE_INHIBIT,			///< @brief --
			MESSAGE_KYBDLOCK		= LIB3270_MESSAGE_KYBDLOCK,			///< @brief Keyboard is locked

			MESSAGE_X				= LIB3270_MESSAGE_X,				///< @brief --
			MESSAGE_RESOLVING		= LIB3270_MESSAGE_RESOLVING,		///< @brief Resolving hostname (running DNS query)
			MESSAGE_CONNECTING		= LIB3270_MESSAGE_CONNECTING		///< @brief Connecting to host

		};

		/// @brief connection state.
		enum ConnectionState : uint8_t {
			DISCONNECTED		= LIB3270_NOT_CONNECTED,				///< @brief disconnected
			RESOLVING			= LIB3270_CONNECTING,					///< @brief Connecting to host
			PENDING				= LIB3270_PENDING,						///< @brief connection pending
			CONNECTED_INITIAL	= LIB3270_CONNECTED_INITIAL,			///< @brief connected, no mode yet
			CONNECTED_ANSI		= LIB3270_CONNECTED_ANSI,				///< @brief connected in NVT ANSI mode
			CONNECTED_3270		= LIB3270_CONNECTED_3270,				///< @brief connected in old-style 3270 mode
			CONNECTED_INITIAL_E	= LIB3270_CONNECTED_INITIAL_E,			///< @brief connected in TN3270E mode, unnegotiated
			CONNECTED_NVT		= LIB3270_CONNECTED_NVT,				///< @brief connected in TN3270E mode, NVT mode
			CONNECTED_SSCP		= LIB3270_CONNECTED_SSCP,				///< @brief connected in TN3270E mode, SSCP-LU mode
			CONNECTED_TN3270E	= LIB3270_CONNECTED_TN3270E,			///< @brief connected in TN3270E mode, 3270 mode
		};

		/// @brief Security state of the connection with the host.
		enum SSLState : uint8_t {
			SSL_UNSECURE	= LIB3270_SSL_UNSECURE,                   	///< @brief No secure connection
			SSL_SECURE		= LIB3270_SSL_SECURE,						///< @brief Connection secure with CA check
			SSL_NEGOTIATED	= LIB3270_SSL_NEGOTIATED,					///< @brief Connection secure, no CA, self-signed or expired CRL
			SSL_NEGOTIATING	= LIB3270_SSL_NEGOTIATING,					///< @brief Negotiating SSL
			SSL_VERIFYING	= LIB3270_SSL_VERIFYING,					///< @brief Verifying SSL (Getting CRL)
			SSL_UNDEFINED	= LIB3270_SSL_UNDEFINED						///< @brief Undefined
		};

		/// @brief PF Key
		enum PFKey : uint8_t {
			PF_1,
			PF_2,
			PF_3,
			PF_4,
			PF_5,
			PF_6,
			PF_7,
			PF_8,
			PF_9,
			PF_10,
			PF_11,
			PF_12,
			PF_13,
			PF_14,
			PF_15,
			PF_16,
			PF_17,
			PF_18,
			PF_19,
			PF_20,
			PF_21,
			PF_22,
			PF_23,
			PF_24
		};

		/// @brief PA Key
		enum PAKey : uint8_t {
			PA_1,
			PA_2,
			PA_3
		};

		/// @brief Keyboard Actions.
		enum KeyboardAction : uint8_t {
			ENTER,				///< @brief Enter key
			ERASE,
			ERASE_EOF,
			ERASE_EOL,
			ERASE_INPUT,
			KYBD_RESET,
			NEWLINE,
			CLEAR,
			SELECT_FIELD,
			SELECT_ALL,
			UNSELECT,
			RESELECT,
			CHAR_DELETE,
			DUP,
			FIELDMARK,
			BACKSPACE,
			WORD_PREVIOUS,
			WORD_NEXT,
			FIELD_END,		///< @brief Move the cursor to the first blank after the last nonblank in the field.
			FIELD_FIRST,	///< @brief Move to first unprotected field on screen.
			FIELD_NEXT,		///< @brief Jump to the next field.
			FIELD_PREVIOUS,	///< @brief Tab backward to previous field.
			ATTN,			///< @brief ATTN key, per RFC 2355.  Sends IP, regardless.
			BREAK,
			WORD_DELETE,	///< @brief Backspaces the cursor until it hits the front of a word (does a ^W).
			FIELD_DELETE,	///< @brief Delete field key (does a ^U).
			SYSREQ,
			KYBD_UNLOCK,	///< @brief Unlock the keyboard if it was locked by operator error.
		};

	}

	/// @brief Convert the program message to a human readable string.
	///
	/// @param programMessage	The program message code.
	///
	/// @return Description of the program message.
	TN3270_PUBLIC const char * toCharString(const TN3270::ProgramMessage programMessage);

	/// @brief Convert the connection state to a human readable string.
	///
	/// @param connectionState	The conection state code.
	///
	/// @return Description of the connection state.
	TN3270_PUBLIC const char * toCharString(const TN3270::ConnectionState connectionState);

	/// @brief Convert the action's description.
	///
	/// @param The action code.
	///
	/// @return The action description.
	TN3270_PUBLIC const char * toCharString(const TN3270::KeyboardAction action);

#endif

#endif // LIB3270_IPC_H_INCLUDED
