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

 #include <config.h>
 #include "private.h"
 #include <lib3270.h>
 #include <lib3270/ipc/field.h>
 #include <memory>

 using namespace std;

 namespace TN3270 {

	std::shared_ptr<Field> Local::Session::FieldFactory(unsigned short row, unsigned short col) const {
		int addr = handler->get<int>([row,col](H3270 * hSession){
			return lib3270_translate_to_address(hSession,row,col);
		});
		return FieldFactory(addr);
	}

	std::shared_ptr<Field> Local::Session::FieldFactory(int baddr) const {

		int from, to;

		int rc = handler->get<int>([baddr,&from,&to](H3270 * hSession){
			return lib3270_get_field_bounds(hSession,baddr,&from,&to);
		});

		if(rc) {
            throw std::system_error((int) rc, std::system_category());
		}

		class LocalField : public TN3270::Field {
		private:
			std::shared_ptr<Handler> handler;

		public:
			LocalField(std::shared_ptr<Handler> h, int from, int to) : TN3270::Field{from, to-from}, handler{h} {
			}

			bool next() override {
				throw std::system_error(ENOTSUP, std::system_category());
			}

		};

		return make_shared<LocalField>(handler,from,to);
	}

 }
