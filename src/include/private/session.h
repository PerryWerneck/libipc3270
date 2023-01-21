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
 #include <config.h>
 #include <lib3270/ipc.h>
 #include <lib3270/ipc/session.h>
 #include <string>
 #include <functional>

 #ifdef HAVE_ICONV
	#include <iconv.h>
 #endif

 namespace TN3270 {

	namespace Abstract {

		/// @brief Remote session (IPC).
		class TN3270_PRIVATE Session : public TN3270::Session {
		private:

#ifdef HAVE_ICONV
			struct {

				/// @brief Convert strings from host codepage to local codepage.
				iconv_t local = (iconv_t) (-1);

				/// @brief Convert string from local codepage to host codepage.
				iconv_t	host = (iconv_t) (-1);

			} converter;

			static std::string convertCharset(iconv_t &converter, const char *str, int length);

#else

			LIB3270_ICONV * converter;

#endif

		protected:

			Session(const char *remote_charset = nullptr, const char *local_charset = nullptr);
			virtual ~Session();

			/// @brief Wait.
			void wait(int seconds, const std::function<int()> worker) const;

			/// @brief Setup charsets
			void setCharSet(const char *remote, const char *local);

			/// @brief Converte string recebida do host para o charset atual.
			std::string convertFromHost(const std::string &str) const;

			/// @brief Converte string do charset atual para o charset do host.
			std::string convertToHost(const std::string &str) const;
			std::string convertToHost(const char *text, int length) const;

			// Get strings from lib3270 without charset conversion.
			virtual std::string	get() const;
			virtual std::string	get(int32_t baddr, int32_t len, uint8_t lf) const;
			virtual std::string	get(uint32_t row, uint32_t col, int32_t len, uint8_t lf) const;

			// Set strings to lib3270 without charset conversion.
			virtual void set(const std::string &str);
			virtual void set(int32_t baddr, const std::string &str);
			virtual void set(uint32_t row, uint32_t col, const std::string &str);

		public:

			static std::shared_ptr<Abstract::Session> getRemoteInstance(const char *id, const char *charset);

			// States
			ProgramMessage getProgramMessage() const override;
			ConnectionState getConnectionState() const override;
			SSLState getSSLState() const override;

			// Contents
			std::string	toString(int baddr, int len, char lf) const override;
			std::string	toString(unsigned short row, unsigned short col, int len, char lf) const override;

			void push(const char *text, size_t length) override;
			void push(int baddr, const char *text, int length) override;
			void push(unsigned short row, unsigned short col, const char *text, int length) override;
			void push(const KeyboardAction action) override;

			void pop(int baddr, std::string &text) override;
			void pop(unsigned short row, unsigned short col, std::string &text) override;
			void pop(std::string &text) override;

			// Attributes
			//std::shared_ptr<Abstract::Attribute> getAttribute(const char *name) const override;
			//void getAttributes(std::vector<std::shared_ptr<Abstract::Attribute>> & attributes) const override;

			/// @brief Get the lib3270 version string.
			std::string getVersion() const override;

			/// @brief Get the lib3270 revision string.
			std::string getRevision() const override;

			/// @brief Get the LU name associated with the session, if there is one.
			std::string getAssociatedLUName() const override;

			/// @brief Get the terminal lock state code.
			LIB3270_KEYBOARD_LOCK_STATE getKeyboardLockState() const override;

			std::string getHostURL() const override;
			void setHostURL(const char *url) override;

			void setUnlockDelay(unsigned short delay = 350) override;
			void setTimeout(time_t timeout = 0) override;
			void setLockOnOperatorError(bool lock = true) override;

			unsigned short getScreenWidth() const override;
			unsigned short getScreenHeight() const override;
			unsigned short getScreenLength() const override;

			/// @brief Set cursor address.
			unsigned short setCursor(int addr = -1) override;

			/// @brief Set cursor position.
			unsigned short setCursor(unsigned short row, unsigned short col) override;

			/// @brief Get cursor address
			unsigned short getCursorAddress() override;

			/// @brief Get cursor position.
			struct Cursor getCursorPosition() override;

			/// @brief Set local charset.
			void setCharSet(const char *charset = NULL) override;

			/// @brief Execute action by name.
			void action(const char *action_name) override;

			// Connect/disconnect
			void connect(const char *url = nullptr, time_t seconds = DEFAULT_TIMEOUT) override;
			void disconnect() override;

			/// @brief Wait.
			void wait(time_t seconds) const override;

			/// @brief Wait until session state changes to "ready".
			void waitForReady(time_t timeout = DEFAULT_TIMEOUT) const override;

			/// @brief Wait for connection state.
			void waitForConnectionState(ConnectionState state, time_t timeout = DEFAULT_TIMEOUT) const override;

			/// @brief Wait for screen changes.
			void waitForChange(time_t seconds = DEFAULT_TIMEOUT) const override;

			/// @brief Wait for screen changes.
			LIB3270_KEYBOARD_LOCK_STATE waitForKeyboardUnlock(time_t seconds = DEFAULT_TIMEOUT) const override;

			/// @brief Wait for string.
			void wait(const char *text, int seconds = DEFAULT_TIMEOUT) override;

			/// @brief Wait for string.
			void wait(uint32_t row, uint32_t col, const char *text, int seconds = DEFAULT_TIMEOUT) override;

			/// @brief Wait for string.
			void wait(int addr, const char *text, int seconds = DEFAULT_TIMEOUT) override;

			/// @brief Send PF.
			void pfkey(unsigned short value) override;

			/// @brief Send PA.
			void pakey(unsigned short value) override;

			/// @brief Request print
			void print(LIB3270_CONTENT_OPTION option = LIB3270_CONTENT_ALL) override;

		};

	}

 }


