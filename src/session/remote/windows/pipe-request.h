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

 #ifdef HAVE_CONFIG_H
	#include <config.h>
 #endif // HAVE_CONFIG_H

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

			BOOL transact(LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesRead) {
				std::lock_guard<std::mutex> lock(guard);
				return TransactNamedPipe(hPipe,lpInBuffer,nInBufferSize,lpOutBuffer,nOutBufferSize,lpBytesRead,NULL);
			}

		};

		class Request : public TN3270::Request {
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

			/// @brief Pointer to number of variants in the output buffer.
			uint16_t * outvalues;

			DataBlock * pushBlock(const void *ptr, size_t length);
			DataBlock * getNextBlock() const;

		public:
			Request(std::shared_ptr<Pipe::Handler> h, const TN3270::Request::Type type, const char *name);
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
