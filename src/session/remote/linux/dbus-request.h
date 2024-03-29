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

 namespace TN3270 {

	namespace DBus {

		class Request : public TN3270::Request {
		private:
			DBusConnection *connection;

			/// @brief Message received from server.
			struct {
				DBusMessage		* msg		= NULL;
				DBusMessageIter	  iter;
			} response;

			/// @brief Message who will be sent to server.
			struct {
				DBusMessage		* msg		= NULL;			///< @brief The request message.
				DBusMessageIter	  iter;						///< @brief Arguments iter.
				bool			  variant	= false;		///< @brief Put arguments as variants?
			} request;

			DBus::Request & push(int type, const void *value);

		public:
			Request(DBusConnection *c, const char *id, const Type type, const char *name);

			Request(DBusConnection *c, const std::string &id, const Type type, const char *name) : Request{c,id.c_str(),type,name} {
			}

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

