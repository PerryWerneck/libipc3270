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
 #include <lib3270/ipc.h>
 #include <lib3270/properties.h>
 #include <lib3270/toggle.h>
 #include <cstring>

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	Attribute * Local::Session::getAttribute(const char *name) const {

		// Signed int attribute
		class IntAttribute : public Attribute {
		private:
			H3270 						* hSession;
			const LIB3270_INT_PROPERTY	* descriptor;

		public:
			IntAttribute(H3270 *hSession, const LIB3270_INT_PROPERTY *descriptor) : Attribute(Attribute::Int32) {
				this->hSession = hSession;
				this->descriptor = descriptor;
			}

			std::string getString() const override {
				return std::to_string(getInt32());
			}

			int32_t getInt32() const override {

				errno = 0;
				int value = descriptor->get(hSession);

				if(errno != 0) {
					throw std::system_error(errno, std::system_category());
				}

				return ((int32_t) value) != 0;

			}

			uint32_t getUint32() const override {
				return (uint32_t) descriptor->get(this->hSession);
			}

			bool getBool() const override {
				return getInt32() != 0;
			}

		};

		// Unsigned int attribute
		class UnsignedIntAttribute : public Attribute {
		private:
			H3270 						* hSession;
			const LIB3270_UINT_PROPERTY	* descriptor;

		public:
			UnsignedIntAttribute(H3270 *hSession, const LIB3270_UINT_PROPERTY *descriptor) : Attribute(Attribute::Uint32) {
				this->hSession = hSession;
				this->descriptor = descriptor;
			}

			std::string getString() const override {
				return std::to_string(getInt32());
			}

			int32_t getInt32() const override {

				return (int32_t) getUint32();
			}

			uint32_t getUint32() const override {

				errno = 0;
				unsigned int value = descriptor->get(hSession);

				if(errno != 0) {
					throw std::system_error(errno, std::system_category());
				}

				return ((uint32_t) value) != 0;
			}

			bool getBool() const override {
				return getUint32() != 0;
			}

		};

		// String attribute
		class StringAttribute : public Attribute {
		private:
			H3270 							* hSession;
			const LIB3270_STRING_PROPERTY	* descriptor;

		public:
			StringAttribute(H3270 *hSession, const LIB3270_STRING_PROPERTY *descriptor) : Attribute(Attribute::String) {
				this->hSession = hSession;
				this->descriptor = descriptor;
			}

			std::string getString() const override {

				const char * str = descriptor->get(hSession);

				if(str) {
					return string(str);
				}

				throw std::system_error(errno, std::system_category());

			}

			int32_t getInt32() const override {
				return (int32_t) atoi(getString().c_str());
			}

			uint32_t getUint32() const override {
				return (uint32_t) atol(getString().c_str());
			}

			bool getBool() const override {
				return getUint32() != 0;
			}

		};

		// Boolean attribute
		class BooleanAttribute : public Attribute {
			H3270 						* hSession;
			const LIB3270_INT_PROPERTY	* descriptor;

		public:
			BooleanAttribute(H3270 *hSession, const LIB3270_INT_PROPERTY *descriptor) : Attribute(Attribute::Boolean) {
				this->hSession = hSession;
				this->descriptor = descriptor;
			}

			std::string getString() const override {
				return std::to_string(getInt32());
			}

			int32_t getInt32() const override {

				errno = 0;
				int value = descriptor->get(hSession);

				if(errno != 0) {
					throw std::system_error(errno, std::system_category());
				}

				return ((int32_t) value) != 0;

			}

			uint32_t getUint32() const override {
				return (uint32_t) descriptor->get(this->hSession);
			}

			bool getBool() const override {
				return getInt32() != 0;
			}

		};

		// Toogle attribute
		class ToggleAttribute : public Attribute {
			H3270 			* hSession;
			LIB3270_TOGGLE	  toggle;

		public:
			ToggleAttribute(H3270 *hSession, const LIB3270_TOGGLE toggle) : Attribute(Attribute::Boolean) {
				this->hSession = hSession;
				this->toggle = toggle;
			}

			std::string getString() const override {
				return (lib3270_get_toggle(hSession, toggle) ? "1" : "0");
			}

			int32_t getInt32() const override {
				return (int32_t) lib3270_get_toggle(hSession, toggle);
			}

			uint32_t getUint32() const override {
				return (uint32_t) lib3270_get_toggle(hSession, toggle);
			}

			bool getBool() const override {
				return (lib3270_get_toggle(hSession, toggle) != 0);
			}

		};

		std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);

		// Check for integer properties.
		{
			const LIB3270_INT_PROPERTY * intprop = lib3270_get_int_properties_list();
			for(size_t ix = 0; intprop[ix].name; ix++) {

				if(!strcasecmp(name,intprop[ix].name)) {
					return new IntAttribute(hSession,&intprop[ix]);
				}

			}
		}

		// Check for unsigned int properties
		{
			const LIB3270_UINT_PROPERTY * intprop = lib3270_get_unsigned_properties_list();
			for(size_t ix = 0; intprop[ix].name; ix++) {

				if(!strcasecmp(name,intprop[ix].name)) {
					return new UnsignedIntAttribute(hSession,&intprop[ix]);
				}

			}

		}

		// Check for string properties
		{
			const LIB3270_STRING_PROPERTY * strprop = lib3270_get_string_properties_list();

			for(size_t ix = 0; strprop[ix].name; ix++) {

				if(!strcasecmp(name,strprop[ix].name)) {
					return new StringAttribute(hSession,&strprop[ix]);
				}

			}

		}

		// Check for boolean properties
		{
			LIB3270_TOGGLE toggle = lib3270_get_toggle_id(name);
			if(toggle != (LIB3270_TOGGLE) -1) {

				// Is a Tn3270 toggle, get it!
				return new ToggleAttribute(hSession,toggle);

			}

			const LIB3270_INT_PROPERTY * intprop = lib3270_get_boolean_properties_list();
			for(size_t ix = 0; intprop[ix].name; ix++) {

				if(!strcasecmp(name,intprop[ix].name)) {

					if(!strcasecmp(name,intprop[ix].name)) {
						return new BooleanAttribute(hSession,&intprop[ix]);
					}

				}

			}

		}

		// Not found!
		throw std::runtime_error("Invalid property");

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

	void Local::Session::setLockOnOperatorError(bool lock) {
		std::lock_guard<std::mutex> guard(sync);
		chkResponse(lib3270_set_lock_on_operator_error(hSession,lock ? 1 : 0));
	}

	unsigned short Local::Session::setCursor(int addr) {
		std::lock_guard<std::mutex> lock(sync);

		int rc = lib3270_set_cursor_address(hSession,addr);
		if(rc < 0)
			chkResponse(-rc);

		return (unsigned short) rc;

	}

	unsigned short Local::Session::setCursor(unsigned short row, unsigned short col) {
		std::lock_guard<std::mutex> lock(sync);

		int rc = lib3270_set_cursor_position(hSession,row,col);
		if(rc < 0)
			chkResponse(-rc);

		return (unsigned short) rc;

	}

	unsigned short Local::Session::getCursorAddress() {
		std::lock_guard<std::mutex> lock(sync);

		int rc = lib3270_get_cursor_address(hSession);

		if(!rc)
			chkResponse(errno);

		return rc;
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


