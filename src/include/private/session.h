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

 }

