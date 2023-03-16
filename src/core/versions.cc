/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) 2021 Perry Werneck <perry.werneck@gmail.com>
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

 #include <lib3270/ipc.h>

 TN3270_PUBLIC const char * TN3270::getApplicationId() noexcept {
	return "br.app.pw3270.terminal";
 }
 
 TN3270_PUBLIC const char * TN3270::getProductName() noexcept {
 	return "pw3270";
 }
 
 TN3270_PUBLIC const char * TN3270::getVersion() noexcept {
	return "5.5";
 }

 TN3270_PUBLIC const char * TN3270::getRevision() noexcept {
	return "0";
 }
 
