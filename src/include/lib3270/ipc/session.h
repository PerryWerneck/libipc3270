/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) 2023 Perry Werneck <perry.werneck@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

 #pragma once
 #include <lib3270/ipc.h>
 #include <lib3270/ipc/session.h>
 #include <lib3270/ipc/request.h>
 #include <iostream>
 #include <memory>

 namespace TN3270 {

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

		/// @brief Get an instance of the TN3270 session bound to an existent lib3270 session.
		static std::shared_ptr<Session> getInstance(H3270 *hSession);

		/// @brief Get an instance of the TN3270 session based on the supplied ID.
		/// @param id The session id (nullptr or empty for local session).
		/// @param charset The local charset.
		static std::shared_ptr<Session> getInstance(const char *id = nullptr, const char *charset = nullptr);

		struct Cursor {

			unsigned short row;
			unsigned short col;

			constexpr Cursor(unsigned short r = 0, unsigned short c = 0) : row{r}, col{c} {
			}

		};

		virtual ~Session();

		// States
		virtual ProgramMessage getProgramMessage() const = 0;
		virtual ConnectionState getConnectionState() const = 0;
		virtual SSLState getSSLState() const = 0;

		// Contents
		virtual std::string	toString(int baddr = 0, int len = -1, char lf = '\n') const = 0;
		virtual std::string	toString(unsigned short row, unsigned short col, int len, char lf = '\n') const = 0;

		/// @brief Build field for current cursor address.
		virtual std::shared_ptr<Field> FieldFactory();

		/// @brief Build field for address.
		virtual std::shared_ptr<Field> FieldFactory(int baddr) = 0;

		/// @brief Build field for row, col.
		virtual std::shared_ptr<Field> FieldFactory(unsigned short row, unsigned short col) = 0;

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
		virtual void setTimeout(time_t timeout = 0) = 0;
		virtual void setLockOnOperatorError(bool lock = true) = 0;

		virtual unsigned short getScreenWidth() const = 0;
		virtual unsigned short getScreenHeight() const = 0;
		virtual unsigned short getScreenLength() const = 0;

		/// @brief Set cursor address.
		virtual unsigned short setCursor(int addr = -1) = 0;

		/// @brief Set cursor position.
		virtual unsigned short setCursor(unsigned short row, unsigned short col) = 0;

		/// @brief Get cursor address
		virtual unsigned short getCursorAddress() const = 0;

		/// @brief Get cursor position.
		virtual struct Cursor getCursorPosition() = 0;

		/// @brief Set local charset.
		virtual void setCharSet(const char *charset = NULL) = 0;

		// Connect/disconnect
		void connect(time_t seconds = DEFAULT_TIMEOUT);
		virtual void connect(const char *url = nullptr, time_t seconds = DEFAULT_TIMEOUT) = 0;
		virtual void disconnect() = 0;

		virtual bool connected() const = 0;
		virtual bool ready() const = 0;

		/// @brief Wait.
		virtual void wait(time_t seconds) const = 0;

		/// @brief Wait until session state changes to "ready".
		virtual void waitForReady(time_t timeout = DEFAULT_TIMEOUT) const = 0;

		/// @brief Wait for connection state.
		virtual void waitForConnectionState(ConnectionState state, time_t timeout = DEFAULT_TIMEOUT) const = 0;

		/// @brief Wait for 'online' state.
		virtual void waitForConnected(time_t timeout = DEFAULT_TIMEOUT) const = 0;

		/// @brief Wait for screen changes.
		virtual void waitForChange(time_t seconds = DEFAULT_TIMEOUT) const = 0;

		/// @brief Wait for screen changes.
		virtual LIB3270_KEYBOARD_LOCK_STATE waitForKeyboardUnlock(time_t seconds = DEFAULT_TIMEOUT) const = 0;

		/// @brief Wait for string.
		virtual void wait(const char *text, int seconds = DEFAULT_TIMEOUT) = 0;

		/// @brief Wait for string.
		virtual void wait(uint32_t row, uint32_t col, const char *text, int seconds = DEFAULT_TIMEOUT) = 0;

		/// @brief Wait for string.
		virtual void wait(int32_t addr, const char *text, int seconds = DEFAULT_TIMEOUT) = 0;

		/// @brief Send PF.
		virtual void pfkey(unsigned short value) = 0;

		/// @brief Send PA.
		virtual void pakey(unsigned short value) = 0;

		/// @brief Request print
		virtual void print(LIB3270_CONTENT_OPTION option = LIB3270_CONTENT_ALL) = 0;

		/// @brief Insert event listener.
		// void insert(Event::Type type, std::function <void(const Event &event)> listener);

		/// @brief Create an action object.
		virtual std::shared_ptr<Action> ActionFactory(const LIB3270_ACTION *descriptor) = 0;

		/// @brief Create an action object.
		std::shared_ptr<Action> ActionFactory(const char *name);

		void activate(const char *name);
		bool activatable(const char *name);

		/// @brief Checks if the terminal contains the string.
		bool contains(const char *str);

		/// @brief Search for string on terminal.
		size_t find(const char * str, size_t pos = 0) const;

		/// @brief Get the number of occurrences of a string in the terminal.
		size_t count(const char * str, size_t pos = 0) const;

		/// @brief Compare contents.
		int compare(int baddr, const char* s, int len = -1) const;
		int compare(unsigned short row, unsigned short col, const char* s, int len = -1) const;

		virtual void setProperty(const char *name, const int value);
		virtual void setProperty(const char *name, const unsigned int value);
		virtual void setProperty(const char *name, const bool value);
		virtual void setProperty(const char *name, const char *value);

		/// @brief Get Request.
		virtual std::shared_ptr<Request> RequestFactory(const Request::Type type, const char *name) const;

		/// @brief Get a single value from remote, convert it to local value.
		/// @tparam F The data type from source (usually uint32_t).
		/// @tparam T The local data type.
		/// @return The remote property converted to local type.
		template <typename F, typename T>
		T getProperty(const char *name) const {
			F value;
			RequestFactory(Request::GetProperty,name)->call().pop(value);
			return (T) value;
		}

		/// @brief Set a single value on remote remote.
		/// @tparam T The remote data type.
		template <typename T>
		void setProperty(const char *name, T value) const {
			RequestFactory(Request::SetProperty,name)->push(value).call();
		}


	};


 }

 namespace std {

	template <typename T>
	inline TN3270_PUBLIC TN3270::Session & operator<<(TN3270::Session& session, const T value) {
		return session.push(value);
	}

	template <typename T>
	inline TN3270_PUBLIC TN3270::Session & operator>>(TN3270::Session& session, const T value) {
		return session.pop(value);
	}

 }

