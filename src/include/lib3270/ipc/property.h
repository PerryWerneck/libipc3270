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
 #include <memory>

 namespace TN3270 {

	class TN3270_PUBLIC Property {
	public:

		enum Type : uint8_t {
			Integer = 'I',
			Unsigned = 'U',
			String = 'S',
			Boolean = 'B'
		};

		virtual const char * name() const = 0;
		virtual const char * description() const = 0;
		virtual const char * label() const = 0;
		virtual const char * icon() const = 0;
		virtual const char * summary() const = 0;
		virtual Type type() const = 0;

		static bool for_each(const std::function<bool(Property &property)> &method);
		static std::shared_ptr<Property> find(const char *name);

	};


 }

