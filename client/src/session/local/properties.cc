/*
 * "Software pw3270, desenvolvido com base nos códigos fontes do WC3270  e X3270
 * (Paul Mattes Paul.Mattes@usa.net), de emulação de terminal 3270 para acesso a
 * aplicativos mainframe. Registro no INPI sob o nome G3270.
 *
 * Copyright (C) <2008> <Banco do Brasil S.A.>
 *
 * Este programa é software livre. Você pode redistribuí-lo e/ou modificá-lo sob
 * os termos da GPL v.2 - Licença Pública Geral  GNU,  conforme  publicado  pela
 * Free Software Foundation.
 *
 * Este programa é distribuído na expectativa de  ser  útil,  mas  SEM  QUALQUER
 * GARANTIA; sem mesmo a garantia implícita de COMERCIALIZAÇÃO ou  de  ADEQUAÇÃO
 * A QUALQUER PROPÓSITO EM PARTICULAR. Consulte a Licença Pública Geral GNU para
 * obter mais detalhes.
 *
 * Você deve ter recebido uma cópia da Licença Pública Geral GNU junto com este
 * programa; se não, escreva para a Free Software Foundation, Inc., 51 Franklin
 * St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Este programa está nomeado como - e possui - linhas de código.
 *
 * Contatos:
 *
 * perry.werneck@gmail.com	(Alexandre Perry de Souza Werneck)
 * erico.mendonca@gmail.com	(Erico Mascarenhas Mendonça)
 *
 */

/**
 * @file
 *
 * @brief
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include "private.h"
 #include <lib3270/properties.h>
 #include <lib3270/toggle.h>
 #include <cstring>

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	void Local::Session::getProperty(const char *name, int &value) const {

		const LIB3270_INT_PROPERTY * intprop = lib3270_get_int_properties_list();
		for(size_t ix = 0; intprop[ix].name; ix++) {

			if(!strcasecmp(name,intprop[ix].name)) {

				std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);

				value = intprop[ix].get(hSession);

				if(value < 0 && errno != 0) {
					throw std::system_error(errno, std::system_category());
				}


			}

		}

		throw std::system_error(ENOENT, std::system_category());
	}

	void Local::Session::getProperty(const char *name, unsigned int &value) const {

		const LIB3270_UINT_PROPERTY * intprop = lib3270_get_unsigned_properties_list();
		for(size_t ix = 0; intprop[ix].name; ix++) {

			if(!strcasecmp(name,intprop[ix].name)) {

				std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);

				value = intprop[ix].get(hSession);

				if(value < 0 && errno != 0) {
					throw std::system_error(errno, std::system_category());
				}


			}

		}

		throw std::system_error(ENOENT, std::system_category());
	}

	void Local::Session::getProperty(const char *name, std::string &value) const {

		const LIB3270_STRING_PROPERTY * strprop = lib3270_get_string_properties_list();

		for(size_t ix = 0; strprop[ix].name; ix++) {

			if(!strcasecmp(name,strprop[ix].name)) {

				std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);

				// Found it!
				const char * str = strprop[ix].get(hSession);

				if(str) {
					value.assign(str);
					return;
				}

				throw std::system_error(errno, std::system_category());

			}

		}

		throw std::system_error(ENOENT, std::system_category());

	}

	void Local::Session::getProperty(const char *name, bool &value) const {

		LIB3270_TOGGLE toggle = lib3270_get_toggle_id(name);
		if(toggle != (LIB3270_TOGGLE) -1) {

			// Is a Tn3270 toggle, get it!
			std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);
			value = lib3270_get_toggle(hSession,toggle);

		}

		throw std::system_error(ENOENT, std::system_category());

	}

	void Local::Session::setProperty(const char *name, const int value) {
		throw std::system_error(ENOTSUP, std::system_category());
	}

	void Local::Session::setProperty(const char *name, const char *value) {
		throw std::system_error(ENOTSUP, std::system_category());
	}

	void Local::Session::setCharSet(const char *charset) {
		Abstract::Session::setCharSet(lib3270_get_display_charset(this->hSession),charset);
	}

	unsigned short Local::Session::getScreenWidth() const {
		std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);
		return (unsigned short) lib3270_get_width(hSession);
	}

	unsigned short Local::Session::getScreenHeight() const {
		std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);
		return (unsigned short) lib3270_get_height(hSession);
	}

	unsigned short Local::Session::getScreenLength() const {
		std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);
		return (unsigned short) lib3270_get_length(hSession);
	}

	void Local::Session::setUnlockDelay(unsigned short delay) {
		std::lock_guard<std::mutex> lock(sync);
		chkResponse(lib3270_set_unlock_delay(hSession,delay));
	}

	void Local::Session::setCursor(unsigned short addr) {
		std::lock_guard<std::mutex> lock(sync);
		chkResponse(lib3270_set_cursor_address(hSession,addr));
	}

	void Local::Session::setCursor(unsigned short row, unsigned short col) {
		std::lock_guard<std::mutex> lock(sync);
		chkResponse(lib3270_set_cursor_position(hSession,row,col));
	}

	unsigned short Local::Session::getCursorAddress() {
		std::lock_guard<std::mutex> lock(sync);
		return lib3270_get_cursor_address(hSession);
	}

	std::string Local::Session::getVersion() const {

		std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);
		return lib3270_get_version();

	}

	std::string Local::Session::getRevision() const {

		std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);
		return lib3270_get_revision();

	}

	std::string Local::Session::getLUName() const {

		std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);
		return lib3270_get_luname(hSession);

	}

	std::string Local::Session::getHostURL() const {

		std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);
		return lib3270_get_url(hSession);

	}

	void Local::Session::setHostURL(const char *url) {

		std::lock_guard<std::mutex> lock(sync);
		chkResponse(lib3270_set_url(hSession, url));

	}


 }


