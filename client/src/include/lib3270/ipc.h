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

		#define DEFAULT_TIMEOUT 5

		/**
		 * @brief Get IPC module library version.
		 *
		 * @return Constant string with the IPC module library version (It's not the same of lib3270).
		 *
		 */
		TN3270_PUBLIC const char * getVersion();

		/**
		 * @brief Get IPC module library revision.
		 *
		 * @return Constant string with the IPC module library revision (It's not the same of lib3270).
		 *
		 */
		TN3270_PUBLIC const char * getRevision();

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
		TN3270_PUBLIC std::vector<const LIB3270_PROPERTY *> getAttributes();

		/**
		 * @brief Get list of actions.
		 *
		 */
		TN3270_PUBLIC std::vector<const LIB3270_ACTION *> getActions();

		/**
		 * @brief Lib3270 dynamic memory block wrapper.
		 *
		 */
		template<typename T>
		class lib3270_auto_cleanup {
		private:
			T *data;

		public:
			lib3270_auto_cleanup(T *data) {
				this->data = data;
			}

			~lib3270_auto_cleanup() {
				lib3270_free((void *) this->data);
			}

			operator bool() const noexcept {
				return this->data != NULL;
			}

			T * operator->() {
				return this->data;
			}

			operator T *() const noexcept {
				return this->data;
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

		/// @brief Security state of the connection with the host.
		enum SSLState : uint8_t {
			SSL_UNSECURE	= LIB3270_SSL_UNSECURE,                   	///< @brief No secure connection
			SSL_SECURE		= LIB3270_SSL_SECURE,						///< @brief Connection secure with CA check
			SSL_NEGOTIATED	= LIB3270_SSL_NEGOTIATED,					///< @brief Connection secure, no CA, self-signed or expired CRL
			SSL_NEGOTIATING	= LIB3270_SSL_NEGOTIATING,					///< @brief Negotiating SSL
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

		/// @brief Dynamic Data type
		class TN3270_PUBLIC Attribute {
		public:

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

		private:
			Type 	  type;			///< @brief Data type.

		protected:

			size_t	  szData;
			uint8_t	* data;			///< @brief Internal worker.

			struct {
				std::function<const char *(const void *worker)> name;
				std::function<const char *(const void *worker)> description;

				std::function <std::string (const Attribute & attr, const void *worker)> asString;
				std::function <int32_t (const Attribute & attr, const void *worker)> asInt32;
				std::function <uint32_t (const Attribute & attr, const void *worker)> asUint32;
				std::function <bool (const Attribute & attr, const void *worker)> asBoolean;
			} get;

			struct {
				std::function <void (const Attribute & attr, const void *worker, const char *value)> asString;
				std::function <void (const Attribute & attr, const void *worker, const int32_t value)> asInt32;
				std::function <void (const Attribute & attr, const void *worker, const uint32_t value)> asUint32;
				std::function <void (const Attribute & attr, const void *worker, const bool value)> asBoolean;
			} set;

			Attribute(Type type, size_t szWorker = 0);

		public:

			Attribute(const Attribute &src);
			Attribute(const Attribute *src);

			~Attribute();

			inline const char * getName() const {
				return this->get.name(this->data);
			}

			inline const char * getDescription() const {
				return this->get.description(this->data);
			}

			inline std::string getString() const {
				return get.asString(*this,data);
			}

			inline int32_t getInt32() const {
				return get.asInt32(*this,data);
			}

			inline uint32_t getUint32() const {
				return get.asUint32(*this,data);
			}

			inline bool getBoolean() const {
				return get.asBoolean(*this,data);
			}

			inline std::string toString() const {
				return get.asString(*this, data);
			}

			inline void setString(const char * value) {
				set.asString(*this,data,value);
			}

			inline void setInt32(const int32_t value) {
				printf("\n%s %d\n\n",__FUNCTION__,value);
				set.asInt32(*this,data,value);
			}

			inline void setUint32(const uint32_t value) {
				set.asUint32(*this,data,value);
			}

			inline void setBoolean(const bool value) {
				set.asBoolean(*this,data,value);
			}

			inline Attribute & operator=(const char * value) {
				set.asString(*this,data,value);
				return *this;
			}

			inline Attribute & operator=(const int32_t value) {
				set.asInt32(*this,data,value);
				return *this;
			}

			inline Attribute & operator=(const uint32_t value) {
				set.asUint32(*this,data,value);
				return *this;
			}

			inline Attribute & operator=(const bool value) {
				set.asBoolean(*this,data,value);
				return *this;
			}

			inline bool operator==(Type type) const noexcept {
				return this->type == type;
			}

			inline Type getType() const {
				return this->type;
			}

			inline operator Type() const {
				return this->type;
			}


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
			static Session * getInstance(const char *id = nullptr, const char *charset = nullptr);
			virtual ~Session();

			// States
			virtual ProgramMessage getProgramMessage() const = 0;
			virtual ConnectionState getConnectionState() const = 0;
			virtual SSLState getSSLState() const = 0;

			// Contents
			virtual std::string	toString(int baddr = 0, int len = -1, char lf = '\n') const = 0;
			virtual std::string	toString(unsigned short row, unsigned short col, int len, char lf = '\n') const = 0;

			/// @brief Input string.
			virtual void push(const char *text, size_t length) = 0;
			virtual void push(int baddr, const char *text, int length) = 0;
			virtual void push(unsigned short row, unsigned short col, const char *text, int length) = 0;

			inline void push(const std::string &text) {
				push(-1,text.c_str(),text.size());
			}

			inline void push(int baddr, const std::string &text) {
				push(baddr,text.c_str(),text.size());
			}

			inline void push(unsigned short row, unsigned short col, const std::string &text) {
				push(row,col,text.c_str(),text.size());
			}

			void push(const PFKey key);
			void push(const PAKey key);
			virtual void push(const KeyboardAction action) = 0;

			/// @brief Get contents of field ad address.
			virtual void pop(int baddr, std::string &text) = 0;

			/// @brief Get contents of field at position.
			virtual void pop(unsigned short row, unsigned short col, std::string &text) = 0;

			/// @brief Get contents of field at cursor position.
			virtual void pop(std::string &text) = 0;

			/// @brief Input string parsing control char.
			///
			/// Insert string parsing the action codes prefixed with the defined control character.
			///
			/// Value | Action      | Description                                                |
			/// :----:|:------------|:-----------------------------------------------------------|
			///  @@P  | -           | Print the screen contents (if available)                   |
			///  @@@@ | -           | Input the @@ char.                                         |
			///  @@E  | ENTER       | -                                                          |
			///  @@F  | ERASE_EOF   | -                                                          |
			///  @@1  | PF1         | Send the PF1 key.                                          |
			///  @@2  | PF2         | Send the PF2 key.                                          |
			///  @@3  | PF3         | Send the PF3 key.                                          |
			///  @@4  | PF4         | Send the PF4 key.                                          |
			///  @@5  | PF5         | Send the PF5 key.                                          |
			///  @@6  | PF6         | Send the PF6 key.                                          |
			///  @@7  | PF7         | Send the PF7 key.                                          |
			///  @@8  | PF8         | Send the PF8 key.                                          |
			///  @@9  | PF9         | Send the PF9 key.                                          |
			///  @@a  | PF10        | Send the PF10 key.                                         |
			///  @@b  | PF11        | Send the PF11 key.                                         |
			///  @@c  | PF12        | Send the PF12 key.                                         |
			///  @@d  | PF13        | Send the PF13 key.                                         |
			///  @@e  | PF14        | Send the PF14 key.                                         |
			///  @@f  | PF15        | Send the PF15 key.                                         |
			///  @@g  | PF16        | Send the PF16 key.                                         |
			///  @@h  | PF17        | Send the PF17 key.                                         |
			///  @@u  | PF18        | Send the PF18 key.                                         |
			///  @@j  | PF19        | Send the PF19 key.                                         |
			///  @@k  | PF20        | Send the PF20 key.                                         |
			///  @@l  | PF21        | Send the PF21 key.                                         |
			///  @@m  | PF22        | Send the PF22 key.                                         |
			///  @@n  | PF23        | Send the PF23 key.                                         |
			///  @@o  | PF24        | Send the PF24 key.                                         |
			///  @@x  | PA1         | Send the PA1 key.                                          |
			///  @@y  | PA2         | Send the PA2 key.                                          |
			///  @@z  | PA3         | Send the PA3 key.                                          |
			///  @@D  | CHAR_DELETE |                                                            |
			///  @@N  | NEWLINE     |                                                            |
			///  @@C  | CLEAR       |                                                            |
			///  @@R  | KYBD_RESET  |                                                            |
			///  @@<  | BACKSPACE   |                                                            |
			///
			/// @return The keyboard lock state.
			///
			LIB3270_KEYBOARD_LOCK_STATE input(const std::string &str, const char control_char = '@');

			// Attributes
			virtual Attribute getAttribute(const char *name) const = 0;
			virtual void getAttributes(std::vector<Attribute> & attributes) const = 0;
			std::vector<Attribute> getAttributes() const;

			virtual void getAttribute(const char *name, int &value) const;
			virtual void getAttribute(const char *name, unsigned int &value) const;
			virtual void getAttribute(const char *name, std::string &value) const;
			virtual void getAttribute(const char *name, bool &value) const;

			virtual void setAttribute(const char *name, const int value);
			virtual void setAttribute(const char *name, const char *value);

			/// @brief Get the lib3270 version string.
			virtual std::string getVersion() const = 0;

			/// @brief Get the lib3270 revision string.
			virtual std::string getRevision() const = 0;

			/// @brief Get the LU name associated with the session, if there is one.
			virtual std::string getAssociatedLUName() const = 0;

			/// @brief Get the terminal lock state code.
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

			/// @brief Wait for string.
			virtual void wait(const char *text, int seconds = DEFAULT_TIMEOUT) = 0;

			/// @brief Wait for string.
			virtual void wait(unsigned short row, unsigned short col, const char *text, int seconds = DEFAULT_TIMEOUT) = 0;

			/// @brief Wait for string.
			virtual void wait(int addr, const char *text, int seconds = DEFAULT_TIMEOUT) = 0;

			/// @brief Send PF.
			virtual void pfkey(unsigned short value) = 0;

			/// @brief Send PA.
			virtual void pakey(unsigned short value) = 0;

			/// @brief Request print
			virtual void print(LIB3270_CONTENT_OPTION option = LIB3270_CONTENT_ALL) = 0;

			/// @brief Insert event listener.
			// void insert(Event::Type type, std::function <void(const Event &event)> listener);

			/// @brief Create an action object.
			virtual Action * getAction(const LIB3270_ACTION *descriptor);

			/// @brief Create an action object.
			Action * getAction(const char *name);

			/// @brief Checks if the terminal contains the string.
			size_t find(const char * str, size_t pos = 0) const;

			/// @brief Get the number of occurrences of a string in the terminal.
			size_t count(const char * str, size_t pos = 0) const;

			/// @brief Compare contents.
			int compare(int baddr, const char* s, int len = -1) const;
			int compare(unsigned short row, unsigned short col, const char* s, int len = -1) const;

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
			Host(const char *id, const char *charset = nullptr);

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
			Host & push(unsigned short row, unsigned short col, const std::string &text);
			Host & push(const std::string &text);

			Host & push(int baddr, const char *str, int len = -1);
			Host & push(unsigned short row, unsigned short col, const char *str, int len = -1);
			Host & push(const char *str, int len = -1);

			Host & pop(int baddr, std::string &text);
			Host & pop(unsigned short row, unsigned short col, std::string &text);
			Host & pop(std::string &text);

			std::string toString() const;
			std::string toString(int baddr, int len = -1, char lf = '\n') const;
			std::string toString(unsigned short row, unsigned short col, int len = -1, char lf = '\n') const;

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
			inline Host & setCursor(int addr = -1) {
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

			inline void setHostURL(const char *url) {
				session->setHostURL(url);
			}


			// Get properties
			Attribute getAttribute(const char *name) const;
			std::vector<Attribute> getAttributes() const;

			inline time_t getTimeout() const noexcept {
				return this->timeout;
			}

			inline Attribute operator[](const char *name) const {
				return getAttribute(name);
			}

			/// @brief Get lib3270 version.
			inline std::string getVersion() const {
				return session->getVersion();
			}

			/// @brief Get lib3270 revision.
			std::string getRevision() const {
				return session->getRevision();
			}

			/// @brief Get LU Name.
			std::string getAssociatedLUName() const {
				return session->getAssociatedLUName();
			}

			inline std::string getHostURL() const {
				return session->getHostURL();
			}

			inline LIB3270_KEYBOARD_LOCK_STATE getKeyboardLockState() const {
				return session->getKeyboardLockState();
			}

			inline SSLState getSSLState() const {
				return session->getSSLState();
			}

			inline unsigned short getScreenWidth() const {
				return session->getScreenWidth();
			}

			inline unsigned short getScreenHeight() const {
				return session->getScreenHeight();
			}

			inline unsigned short getScreenLength() const {
				return session->getScreenLength();
			}

			// Set properties
			inline void setTimeout(time_t timeout = DEFAULT_TIMEOUT) noexcept {
				this->timeout = timeout;
			}

			inline void setUnlockDelay(unsigned short delay = 350) {
				session->setUnlockDelay(delay);
			}

			inline void setLockOnOperatorError(bool lock = true) {
				session->setLockOnOperatorError(lock);
			}

			inline void setCharSet(const char *charset = NULL) {
				session->setCharSet(charset);
			}


			// Actions

			/// @brief Create new action object.
			///
			/// Alocate a new action object associated with the session, delete it
			/// when no longer necessary.
			///
			inline TN3270::Action * getAction(const LIB3270_ACTION *descriptor) {
				return session->getAction(descriptor);
			}

			/// @brief Create new action object.
			///
			/// Alocate a new action object associated with the session, delete it
			/// when no longer necessary.
			///
			inline TN3270::Action * getAction(const char *name) {
				return session->getAction(name);
			}

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

			/// @brief Wait for string.
			inline void wait(const char *text) {
				return session->wait(text,this->timeout);
			}

			/// @brief Wait for string.
			inline void wait(unsigned short row, unsigned short col, const char *text) {
				return session->wait(row,col,text,this->timeout);
			}

			inline void wait(unsigned short row, unsigned short col, const char *text, time_t timeout) {
				return session->wait(row,col,text,timeout);
			}

			/// @brief Wait for string.
			inline void wait(int addr, const char *text) {
				return session->wait(addr,text,this->timeout);
			}

			inline void wait(int addr, const char *text, time_t timeout) {
				return session->wait(addr,text,timeout);
			}

			/// @brief Checks if the terminal contains the string.
			size_t find(const char * str, size_t pos = 0) const;

			/// @brief Get the number of occurrences of a string in the terminal.
			size_t count(const char * str, size_t pos = 0) const;

			/// @brief Compare contents.
			int compare(int baddr, const char* s, int len = -1) const;
			int compare(unsigned short row, unsigned short col, const char* s, int len = -1) const;

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


			/*
			// Event listeners
			inline Host & insert(Event::Type type, std::function <void(const Event &event)> listener) noexcept {
				session->insert(type, listener);
				return *this;
			}
			*/


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

	inline std::ostream & operator<<(std::ostream &stream, const TN3270::Attribute& attribute) {
        stream << attribute.toString();
        return stream;
	}

#endif

#endif // LIB3270_IPC_H_INCLUDED
