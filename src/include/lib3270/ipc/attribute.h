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

 namespace TN3270 {

	/*
	namespace Abstract {

		class TN3270_PUBLIC Attribute {
		protected:

			inline int as_bool(bool val) const {
				return val;
			}

			inline int as_int(int val) const {
				return val;
			}

			inline unsigned int as_uint(unsigned int val) const {
				return val;
			}

			inline const std::string as_string(const std::string &val) const {
				return val;
			}

		public:
			virtual const char * name() const;
			virtual const char * description() const;

			virtual void set(const bool value);
			virtual void set(const int value);
			virtual void set(const uint value);
			virtual void set(const std::string &value);
			virtual void set(const char *value);

			virtual bool as_bool();
			virtual int as_int() const = 0;
			virtual unsigned int as_uint() const = 0;
			virtual std::string as_string() const = 0;

		};

	}

	template <typename T>
	class TN3270_PUBLIC Attribute {
	private:
		T value;

	public:
		constexpr Attribute(T val) : value{val} {
		}

		void set(const T value) override {
			this->value = value;
		}

		int as_bool() const override {
			return as_bool(value);
		}

		int as_int() const override {
			return as_int(value);
		}

		unsigned int as_uint() const override {
			return as_uint(value);
		}

		const std::string as_string() const override {
			return as_string(value);
		}

	};
	*/

 }

