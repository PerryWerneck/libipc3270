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

 #ifdef HAVE_CONFIG_H
	#include <config.h>
 #endif // HAVE_CONFIG_H

 #include "private.h"
 #include <lib3270.h>
 #include <lib3270/ipc/field.h>
 #include <memory>

 using namespace std;

 namespace TN3270 {

	std::shared_ptr<Field> Local::Session::FieldFactory(unsigned short row, unsigned short col) {
		int addr = handler->get<int>([row,col](H3270 * hSession){
			return lib3270_translate_to_address(hSession,row,col);
		});
		return FieldFactory(addr);
	}

	std::shared_ptr<Field> Local::Session::FieldFactory(int baddr) {

		int from, to;
		LIB3270_FIELD_ATTRIBUTE attr;

		int rc = handler->get<int>([baddr,&from,&to,&attr](H3270 * hSession){

			if(lib3270_get_field_bounds(hSession,baddr,&from,&to)) {
				return errno;
			}

			attr = lib3270_get_field_attribute(hSession,baddr);
			if(attr == LIB3270_FIELD_ATTRIBUTE_INVALID)
				return errno;

			return 0;

		});

		if(rc) {
            throw std::system_error((int) rc, std::system_category());
		}

		class LocalField : public TN3270::Field {
		public:
			LocalField(Local::Session &session, int from, int to) : TN3270::Field{session, (unsigned short) from, (unsigned short) (to-from)} {
			}

			bool next() override {
				throw std::system_error(ENOTSUP, std::system_category());
			}

		};

		return make_shared<LocalField>(*this,from,to);
	}

 }
