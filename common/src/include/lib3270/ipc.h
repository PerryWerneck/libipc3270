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
			FIELD_NEXT,
			FIELD_PREVIOUS,	///< @brief Tab backward to previous field.
			ATTN,			///< @brief ATTN key, per RFC 2355.  Sends IP, regardless.
			BREAK,
			WORD_DELETE,	///< @brief Backspaces the cursor until it hits the front of a word (does a ^W).
			FIELD_DELETE,	///< @brief Delete field key (does a ^U).
			SYSREQ
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

			/// @brief Create a tn3270 session.
			static Session * create(const char *id = nullptr);

			virtual ~Session();

			// Connect/disconnect
			virtual void connect(const char *url) = 0;
			virtual void disconnect() = 0;

			// Gets
			virtual std::string	toString(int baddr = 0, size_t len = -1, char lf = '\n') const = 0;
			virtual std::string	toString(int row, int col, size_t sz, char lf = '\n') const = 0;

			inline operator std::string() const {
				return toString();
			}

			// Get properties.
			virtual void getProperty(const char *name, int &value) const = 0;
			virtual void getProperty(const char *name, std::string &value) const = 0;
			virtual void getProperty(const char *name, bool &value) const = 0;

			virtual std::string getVersion() const = 0;
			virtual std::string getRevision() const = 0;
			virtual std::string getLUName() const = 0;
			virtual std::string getHostURL() const = 0;

			virtual unsigned short getScreenWidth() const = 0;
			virtual unsigned short getScreenHeight() const = 0;
			virtual unsigned short getScreenLength() const = 0;

			virtual ProgramMessage getProgramMessage() const = 0;
			inline operator ProgramMessage() const {
				return getProgramMessage();
			}

			virtual ConnectionState getConnectionState() const = 0;
			inline operator ConnectionState() const {
				return getConnectionState();
			}

			inline bool operator==(ConnectionState state) const {
				return this->getConnectionState() == state;
			}

			virtual SSLState getSSLState() const = 0;

			inline operator SSLState() const {
				return getSSLState();
			}

			inline bool operator==(SSLState state) const {
				return this->getSSLState() == state;
			}

			// Set properties.
			virtual void setUnlockDelay(unsigned short delay = 350) = 0;
			void setCharSet(const char *charset);
			virtual void setHostURL(const char *url) = 0;

			// Set contents.

			/// @brief Set field at current posicion, jumps to next writable field.
			virtual Session & push(const char *text) = 0;
			inline Session & push(const std::string &text) {
				return push(text.c_str());
			}

			/// @brief Input string.
			virtual Session & input(const char *text, size_t length) = 0;

			/// @brief Set cursor address.
			virtual TN3270::Session & setCursor(unsigned short addr) = 0;

			/// @brief Set cursor position.
			virtual TN3270::Session & setCursor(unsigned short row, unsigned short col) = 0;

			/// @brief Get cursor address
			virtual unsigned short getCursorAddress() = 0;

			/// @brief Send PF.
			virtual Session & pfkey(unsigned short value) = 0;

			/// @brief Send PA.
			virtual Session & pakey(unsigned short value) = 0;

			virtual Session & push(int baddr, const char *text, int length) = 0;
			virtual Session & push(int row, int col, const char *text, int length) = 0;

			Session & push(int row, int col, const std::string &text);
			Session & push(int baddr, const std::string &text);

			Session & push(const PFKey key);
			Session & push(const PAKey key);

			virtual Session & push(const Action action) = 0;

			// Get contents.
			virtual Session & pop(int baddr, std::string &text) = 0;
			virtual Session & pop(int row, int col, std::string &text) = 0;
			virtual Session & pop(std::string &text) = 0;

			/// @brief Insert event listener.
			void insert(Event::Type type, std::function <void(const Event &event)> listener);

			// Misc

			/// @brief Execute action by name.
			virtual Session & action(const char *action_name) = 0;

			/// @brief Wait.
			virtual Session & wait(unsigned short seconds) = 0;

			/// @brief Wait until session state changes to "ready".
			virtual void waitForReady(time_t timeout = DEFAULT_TIMEOUT) = 0;

			/// @brief Wait for screen changes.
			virtual Session & waitForChange(unsigned short seconds) = 0;

			/// @brief Wait for string.
			///
			/// @return 0 if the string was found, error code if not.
			int wait(int row, int col, const char *key, unsigned short seconds);
			int wait(int baddr, const char *key, unsigned short seconds);

			/// @brief Search
			size_t find(const char * str, size_t pos = 0) const;

			/// @brief Compare contents.
			int compare(size_t baddr, const char* s, size_t len) const;
			int compare(int row, int col, const char* s, size_t len) const;

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

			/// @brief Write information to log file.
			void info(const char *fmt, ...) const;

			/// @brief Write warning to log file.
			void warning(const char *fmt, ...) const;

			/// @brief Write error to log file.
			void error(const char *fmt, ...) const;

		public:
			Host(const char *id = nullptr, const char *url = nullptr, time_t timeout = DEFAULT_TIMEOUT);
			~Host();

			inline bool operator==(ConnectionState state) const noexcept {
				return session->getConnectionState() == state;
			}

			Host & connect(const char *url = nullptr, bool sync = true);
			Host & disconnect();

			Host & waitForReady(time_t timeout = DEFAULT_TIMEOUT);

			/// @brief Execute action by name.
			inline Host & action(const char *action_name) {
				session->action(action_name);
				return *this;
			}

			inline ProgramMessage getProgramMessage() const {
				return session->getProgramMessage();
			}

			inline operator bool() const {
				return isReady();
			}

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
			inline void setCursor(unsigned short addr) {
				session->setCursor(addr);
			}

			inline void setCursorAddress(unsigned short addr) {
				session->setCursor(addr);
			}

			/// @brief Set cursor position.
			inline void setCursor(unsigned short row, unsigned short col) {
				session->setCursor(row,col);
			}

			/// @brief Get cursor address
			inline unsigned short getCursorAddress() {
				return session->getCursorAddress();
			}

			inline void setUnlockDelay(unsigned short delay) {
				return session->setUnlockDelay(delay);
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

			// Actions

			/// @brief Send PF.
			Host & pfkey(unsigned short value);

			/// @brief Send PA.
			Host & pakey(unsigned short value);

			/// @brief Request print
			Host & print(LIB3270_CONTENT_OPTION option = LIB3270_CONTENT_ALL);

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
			Host & input(const char *text, const char control_char = '@');

			Host & input(const char *text, size_t sz);

			/// @brief Set field at current posicion, jumps to next writable field.
			inline Host & push(const char *text) {
				session->push(text);
				return *this;
			};

			inline Host & push(const std::string &text) {
				session->push(text);
				return *this;

			}

			inline Host & push(int baddr, const std::string &text) {
				session->push(baddr,text);
				return *this;
			}

			inline Host & push(int baddr, const char *text, int length = -1) {
				session->push(baddr,text,length);
				return *this;
			}

			inline Host & push(int row, int col, const std::string &text) {
				session->push(row,col,text);
				return *this;
			}

			inline Host & push(int row, int col, const char *text, int length = -1) {
				session->push(row,col,text,length);
				return *this;
			}

			inline Host & push(const PFKey key) {
				session->push(key);
				return *this;
			}

			inline Host & push(const PAKey key) {
				session->push(key);
				return *this;
			}

			Host & push(const Action action);

			// Get contents.

			Host & pop(int baddr, std::string &text);
			Host & pop(int row, int col, std::string &text);
			Host & pop(std::string &text);

			std::string toString() const;
			std::string toString(int baddr, size_t len = -1, char lf = '\n') const;
			std::string toString(int row, int col, size_t sz, char lf = '\n') const;

			// Event listeners
			inline Host & insert(Event::Type type, std::function <void(const Event &event)> listener) noexcept {
				session->insert(type, listener);
				return *this;
			}


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
