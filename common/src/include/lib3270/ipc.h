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

	#if defined(_WIN32)

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

		#define DEFAULT_TIMEOUT 5

		TN3270_PUBLIC const char * getVersion();
		TN3270_PUBLIC const char * getRevision();

		class TN3270_PUBLIC Event {
		public:
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
			RESOLVING			= LIB3270_RESOLVING,					///< @brief resolving hostname
			PENDING				= LIB3270_PENDING,						///< @brief connection pending
			CONNECTED_INITIAL	= LIB3270_CONNECTED_INITIAL,			///< @brief connected, no mode yet
			CONNECTED_ANSI		= LIB3270_CONNECTED_ANSI,				///< @brief connected in NVT ANSI mode
			CONNECTED_3270		= LIB3270_CONNECTED_3270,				///< @brief connected in old-style 3270 mode
			CONNECTED_INITIAL_E	= LIB3270_CONNECTED_INITIAL_E,			///< @brief connected in TN3270E mode, unnegotiated
			CONNECTED_NVT		= LIB3270_CONNECTED_NVT,				///< @brief connected in TN3270E mode, NVT mode
			CONNECTED_SSCP		= LIB3270_CONNECTED_SSCP,				///< @brief connected in TN3270E mode, SSCP-LU mode
			CONNECTED_TN3270E	= LIB3270_CONNECTED_TN3270E,			///< @brief connected in TN3270E mode, 3270 mode
		};

		enum SSLState : uint8_t {
			SSL_UNSECURE	= LIB3270_SSL_UNSECURE,                   	///< @brief No secure connection
			SSL_SECURE		= LIB3270_SSL_SECURE,						///< @brief Connection secure with CA check
			SSL_NEGOTIATED	= LIB3270_SSL_NEGOTIATED,					///< @brief Connection secure, no CA, self-signed or expired CRL
			SSL_NEGOTIATING	= LIB3270_SSL_NEGOTIATING,					///< @brief Negotiating SSL
			SSL_UNDEFINED	= LIB3270_SSL_UNDEFINED						///< @brief Undefined
		};

		/// @brief PF Keys
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

		/// @brief PF Keys
		enum PAKey : uint8_t {
			PA_1,
			PA_2,
			PA_3
		};

		/// @brief Actions keys
		enum Action : uint8_t {
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

		/// @brief TN3270 Session.
		class TN3270_PUBLIC Session {
		protected:
			Session();

			/// @brief Write information to log file.
			void info(const char *fmt, ...) const;

			/// @brief Write warning to log file.
			void warning(const char *fmt, ...) const;

			/// @brief Write error to log file.
			void error(const char *fmt, ...) const;

			/// @brief Fire event.
			void fire(const Event &event);

		public:

			/// @brief Get an instance of the TN3270 session based on the supplied ID.
			static Session * getInstance(const char *id = nullptr);
			virtual ~Session();

			// States
			virtual ProgramMessage getProgramMessage() const = 0;
			virtual ConnectionState getConnectionState() const = 0;
			virtual SSLState getSSLState() const = 0;

			// Contents
			virtual std::string	toString(int baddr = 0, int len = -1, char lf = '\n') const = 0;
			virtual std::string	toString(unsigned int row, unsigned int col, int len, char lf = '\n') const = 0;

			/// @brief Input string.
			virtual void push(const char *text, size_t length) = 0;
			virtual void push(int baddr, const char *text, int length) = 0;
			virtual void push(int row, int col, const char *text, int length) = 0;

			inline void push(const std::string &text) {
				push(-1,text.c_str(),text.size());
			}

			inline void push(int baddr, const std::string &text) {
				push(baddr,text.c_str(),text.size());
			}

			inline void push(int row, int col, const std::string &text) {
				push(row,col,text.c_str(),text.size());
			}

			void push(const PFKey key);
			void push(const PAKey key);
			virtual void push(const Action action) = 0;

			/// @brief Get contents of field ad address.
			virtual void pop(int baddr, std::string &text) = 0;

			/// @brief Get contents of field at position.
			virtual void pop(int row, int col, std::string &text) = 0;

			/// @brief Get contents of field at cursor position.
			virtual void pop(std::string &text) = 0;

			/**
			 * @brief Input string parsing control char.
			 */
			LIB3270_KEYBOARD_LOCK_STATE input(const std::string &str, const char control_char = '@');

			// Properties.
			virtual void getProperty(const char *name, int &value) const = 0;
			virtual void getProperty(const char *name, unsigned int &value) const = 0;
			virtual void getProperty(const char *name, std::string &value) const = 0;
			virtual void getProperty(const char *name, bool &value) const = 0;
			virtual void setProperty(const char *name, const int value) = 0;
			virtual void setProperty(const char *name, const char *value) = 0;

			virtual std::string getVersion() const = 0;
			virtual std::string getRevision() const = 0;
			virtual std::string getLUName() const = 0;
			virtual LIB3270_KEYBOARD_LOCK_STATE getKeyboardLockState() const = 0;

			virtual std::string getHostURL() const = 0;
			virtual void setHostURL(const char *url) = 0;

			virtual void setUnlockDelay(unsigned short delay = 350) = 0;
			virtual void setLockOnOperatorError(bool lock = true) = 0;

			virtual unsigned short getScreenWidth() const = 0;
			virtual unsigned short getScreenHeight() const = 0;
			virtual unsigned short getScreenLength() const = 0;

			/// @brief Set cursor address.
			virtual unsigned short setCursor(int addr = -1) = 0;

			/// @brief Set cursor position.
			virtual unsigned short setCursor(unsigned short row, unsigned short col) = 0;

			/// @brief Get cursor address
			virtual unsigned short getCursorAddress() = 0;

			/// @brief Set local charset.
			virtual void setCharSet(const char *charset = NULL) = 0;

			// Actions

			/// @brief Execute action by name.
			virtual void action(const char *action_name) = 0;

			// Connect/disconnect
			virtual void connect(const char *url = nullptr, int seconds = DEFAULT_TIMEOUT) = 0;
			virtual void disconnect() = 0;

			/// @brief Wait.
			virtual void wait(time_t seconds) const = 0;

			/// @brief Wait until session state changes to "ready".
			virtual void waitForReady(time_t timeout = DEFAULT_TIMEOUT) const = 0;

			/// @brief Wait for screen changes.
			virtual void waitForChange(time_t seconds = DEFAULT_TIMEOUT) const = 0;

			/// @brief Wait for screen changes.
			virtual LIB3270_KEYBOARD_LOCK_STATE waitForKeyboardUnlock(time_t seconds = DEFAULT_TIMEOUT) const = 0;

			/// @brief Send PF.
			virtual void pfkey(unsigned short value) = 0;

			/// @brief Send PA.
			virtual void pakey(unsigned short value) = 0;

			/// @brief Request print
			virtual void print(LIB3270_CONTENT_OPTION option = LIB3270_CONTENT_ALL) = 0;

			/// @brief Insert event listener.
			// void insert(Event::Type type, std::function <void(const Event &event)> listener);

			// Misc

			/// @brief Search
			size_t find(const char * str, size_t pos = 0) const;

			/// @brief Compare contents.
			int compare(int baddr, const char* s, int len = -1) const;
			int compare(unsigned int row, unsigned int col, const char* s, int len = -1) const;

		};

		/// @brief TN3270 Host
		class TN3270_PUBLIC Host : public std::basic_streambuf<char, std::char_traits<char> > {
		private:

			/// @brief Connection with the host
			Session *session;

			/// @brief How much seconds we wait for the terminal to be ready?
			time_t timeout;

		protected:

			/// @brief Writes characters to the associated file from the put area
			int sync() override;

			/// @brief Writes characters to the associated output sequence from the put area.
			int overflow(int c) override;

			/*
			/// @brief Write information to log file.
			void info(const char *fmt, ...) const;

			/// @brief Write warning to log file.
			void warning(const char *fmt, ...) const;

			/// @brief Write error to log file.
			void error(const char *fmt, ...) const;
			*/

		public:
			Host(const char *id = nullptr, const char *url = nullptr, time_t timeout = DEFAULT_TIMEOUT);
			~Host();

			inline bool operator==(ConnectionState state) const noexcept {
				return session->getConnectionState() == state;
			}

			// Contents

			/**
			 * @brief Input string parsing control char.
			 */
			LIB3270_KEYBOARD_LOCK_STATE input(const char *text, int length = -1, const char control_char = '@');

			Host & push(int baddr, const std::string &text);
			Host & push(int row, int col, const std::string &text);
			Host & push(const std::string &text);

			Host & push(int baddr, const char *str, int len = -1);
			Host & push(int row, int col, const char *str, int len = -1);
			Host & push(const char *str, int len = -1);

			Host & pop(int baddr, std::string &text);
			Host & pop(int row, int col, std::string &text);
			Host & pop(std::string &text);

			std::string toString() const;
			std::string toString(int baddr, size_t len = -1, char lf = '\n') const;
			std::string toString(unsigned int row, unsigned int col, size_t len = -1, char lf = '\n') const;

			template<typename T>
			Host & push(T value) {
				session->push(value);
				sync();
				return *this;
			}

			template<typename T>
			Host & pop(T value) {
				sync();
				session->pop(value);
				return *this;
			}

			// Actions
			Host & connect(const char *url = nullptr);
			Host & disconnect();
			Host & waitForReady(time_t seconds);

			inline Host & waitForReady() {
				return waitForReady(this->timeout);
			}

			inline LIB3270_KEYBOARD_LOCK_STATE waitForKeyboardUnlock() noexcept {
				return this->session->waitForKeyboardUnlock(timeout);
			}

			/// @brief Execute action by name.
			inline Host & action(const char *action_name) {
				session->action(action_name);
				return *this;
			}

			inline ProgramMessage getProgramMessage() const {
				return session->getProgramMessage();
			}

			operator bool() const;

			inline operator ProgramMessage() const {
				return getProgramMessage();
			}

			inline ConnectionState getConnectionState() const {
				return session->getConnectionState();
			}

			bool isReady() const;
			bool isConnected() const;

			inline operator ConnectionState() const {
				return getConnectionState();
			}

			/// @brief Set cursor address.
			inline Host & setCursor(unsigned short addr) {
				session->setCursor(addr);
				return *this;
			}

			/// @brief Set cursor position.
			inline Host & setCursor(unsigned short row, unsigned short col) {
				session->setCursor(row,col);
				return *this;
			}

			/// @brief Get cursor address
			inline unsigned short getCursorAddress() {
				return session->getCursorAddress();
			}

			inline void setUnlockDelay(unsigned short delay) {
				return session->setUnlockDelay(delay);
			}

			inline void setLockOnOperatorError(bool lock = true) {
				return session->setLockOnOperatorError(lock);
			}

			inline void setCharSet(const char *charset) {
				return session->setCharSet(charset);
			}

			// Get properties

			/// @brief Get lib3270 version.
			inline std::string getVersion() const {
				return session->getVersion();
			}

			/// @brief Get lib3270 revision.
			std::string getRevision() const {
				return session->getRevision();
			}

			/// @brief Get LU Name.
			std::string getLUName() const {
				return session->getLUName();
			}

			inline LIB3270_KEYBOARD_LOCK_STATE getKeyboardLockState() const {
				return session->getKeyboardLockState();
			}

			// Actions

			/// @brief Send PF.
			inline Host & pfkey(unsigned short value) {
				session->pfkey(value);
				return *this;
			}

			/// @brief Send PA.
			inline Host & pakey(unsigned short value) {
				session->pakey(value);
				return *this;
			}

			/// @brief Request print
			inline Host & print(LIB3270_CONTENT_OPTION option = LIB3270_CONTENT_ALL) {
				session->print(option);
				return *this;
			}

			/// @brief Wait.
			inline Host & wait(unsigned short seconds) {
				session->wait(seconds);
				return *this;
			}

			/// @brief Wait for update.
			inline Host & waitForChange(unsigned short seconds) {
				session->waitForChange(seconds);
				return *this;
			}

			// Set contents.

			/// @brief Input string.
			///
			/// Input the supplied string processing commands delimited by the control char in
			/// the same way of the old HLLAPI.
			///
			/// @param text			String with the text to input.
			/// @param control_char	Control character used to identify commands.
			// Host & input(const char *text, const char control_char = '@');

			// Host & input(const char *text, size_t sz);

			/// @brief Set field at current position, jumps to next writable field.
			inline Host & push(const char *text) {
				session->push(text,-1);
				return *this;
			};

			/*
			// Event listeners
			inline Host & insert(Event::Type type, std::function <void(const Event &event)> listener) noexcept {
				session->insert(type, listener);
				return *this;
			}
			*/


		};

	}

	TN3270_PUBLIC const char * toCharString(const TN3270::ProgramMessage programMessage);
	TN3270_PUBLIC const char * toCharString(const TN3270::ConnectionState connectionState);
	TN3270_PUBLIC const char * toCharString(const TN3270::Action action);

	template <typename T>
	inline TN3270_PUBLIC TN3270::Session & operator<<(TN3270::Session& session, const T value) {
		return session.push(value);
	}

	template <typename T>
	inline TN3270_PUBLIC TN3270::Session & operator>>(TN3270::Session& session, const T value) {
		return session.pop(value);
	}

	template <typename T>
	inline TN3270_PUBLIC TN3270::Host & operator<<(TN3270::Host& host, const T value) {
		return host.push(value);
	}

	inline std::ostream & operator<<(std::ostream &stream, const TN3270::Host& host) {
        stream << host.toString();
        return stream;
	}


#endif

#endif // LIB3270_IPC_H_INCLUDED
