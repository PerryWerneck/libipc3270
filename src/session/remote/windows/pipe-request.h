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
 #include <lib3270/ipc/request.h>
 #include <mutex>
 #include <memory>

 namespace TN3270 {

	namespace Pipe {

		/// @brief Remote pipe.
		class Handler {
		private:
			HANDLE hPipe;
			std::mutex guard;

		public:
			Handler(HANDLE h) : hPipe{h} {
			}

			~Handler() {
				CloseHandle(hPipe);
			}

		};

		class Request : public TN3270::Request {
		private:
			std::shared_ptr<Pipe::Handler> handler;

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

		public:
			Request(std::shared_ptr<Pipe::Handler> h, const Request::Type type, const char *name);

			virtual ~Request();

			TN3270::Request & call() override;

			// Push values
			TN3270::Request & push(const char *arg) override;
			TN3270::Request & push(const bool arg) override;
			TN3270::Request & push(const int32_t arg) override;
			TN3270::Request & push(const uint32_t arg) override;
			TN3270::Request & push(const uint8_t arg) override;

			// Pop values
			TN3270::Request & pop(std::string &value) override;
			TN3270::Request & pop(int &value) override;
			TN3270::Request & pop(unsigned int &value) override;
			TN3270::Request & pop(bool &value) override;

		};

	}

 }
