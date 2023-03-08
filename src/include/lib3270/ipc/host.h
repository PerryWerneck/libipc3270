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
 #include <lib3270/ipc/action.h>
 #include <iostream>

 namespace TN3270 {

	/// @brief TN3270 Host
	class TN3270_PUBLIC Host : public std::basic_streambuf<char, std::char_traits<char> > {
	private:

		/// @brief Connection with the host
		std::shared_ptr<Session> session;

		/// @brief How much seconds we wait for the terminal to be ready?
		time_t timeout;

	protected:

		/// @brief Writes characters to the associated file from the put area
		int sync() override;

		/// @brief Writes characters to the associated output sequence from the put area.
		int overflow(int c) override;

	public:
		Host(const Host &src) = delete;
		Host(const Host *src) = delete;

		Host(const char *id, const char *charset = nullptr);

		~Host();

		inline bool operator==(ConnectionState state) const noexcept {
			return session->getConnectionState() == state;
		}

		/// @brief Create an action object.
		inline std::shared_ptr<Action> ActionFactory(const LIB3270_ACTION *descriptor) {
			return session->ActionFactory(descriptor);
		}

		/// @brief Create an action object.
		inline std::shared_ptr<Action> ActionFactory(const char *name) {
			return session->ActionFactory(name);
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

		/// @brief Wait for connection state.
		inline void waitForConnectionState(ConnectionState state, time_t timeout = DEFAULT_TIMEOUT) {
			return this->session->waitForConnectionState(state,timeout);
		}

		inline void wait(ConnectionState state, time_t timeout = DEFAULT_TIMEOUT) {
			return this->session->waitForConnectionState(state,timeout);
		}

		/// @brief Execute action by name.
		inline Host & action(const char *action_name) {
			session->ActionFactory(action_name)->activate();
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

		inline Session::Cursor getCursorPosition() {
			return session->getCursorPosition();
		}

		inline void setHostURL(const char *url) {
			session->setHostURL(url);
		}


		// Get properties
		// std::shared_ptr<Abstract::Attribute> getAttribute(const char *name) const;
		// std::vector<std::shared_ptr<Abstract::Attribute>> getAttributes() const;

		inline time_t getTimeout() const noexcept {
			return this->timeout;
		}

		//inline std::shared_ptr<Abstract::Attribute> operator[](const char *name) const {
		//	return getAttribute(name);
		//}

		// Set properties.
		inline void setProperty(const char *name, const int value) {
			session->setProperty(name,value);
		}

		inline void setProperty(const char *name, const unsigned int value) {
			session->setProperty(name,value);
		}

		inline void setProperty(const char *name, const bool value) {
			session->setProperty(name,value);
		}

		inline void setProperty(const char *name, const char *value) {
			session->setProperty(name,value);
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
		void setTimeout(time_t timeout = DEFAULT_TIMEOUT);

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

	};

 }

 namespace std {

	template <typename T>
	inline TN3270_PUBLIC TN3270::Host & operator<<(TN3270::Host& host, const T value) {
		return host.push(value);
	}

	inline std::ostream & operator<<(std::ostream &stream, const TN3270::Host& host) {
        stream << host.toString();
        return stream;
	}

 }
