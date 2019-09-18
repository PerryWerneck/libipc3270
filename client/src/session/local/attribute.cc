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

	// Signed int attribute
	class IntAttribute : public Attribute {
	public:
		IntAttribute(H3270 *hSession, const LIB3270_INT_PROPERTY *worker) : Attribute(hSession, Attribute::Int32, (void *) worker) {

			get.asString = [](const Attribute & attr, const void *worker) {
				return std::to_string(attr.getInt32());
			};

			get.asInt32 = [](const Attribute & attr, const void *worker) {

				errno = 0;
				int value = ((const LIB3270_INT_PROPERTY *) worker)->get(attr.getTN3270Session());

				if(errno != 0) {
					throw std::system_error(errno, std::system_category());
				}

				return (int32_t) value;

			};

			get.asUint32 = [](const Attribute & attr, const void *worker) {
				return (uint32_t) attr.getInt32();
			};

			get.asBoolean = [](const Attribute & attr, const void *worker) {
				return (attr.getInt32() != 0);
			};

		}

	};

	// Unsigned int attribute
	class UnsignedIntAttribute : public Attribute {
	public:
		UnsignedIntAttribute(H3270 *hSession, const LIB3270_UINT_PROPERTY *worker) : Attribute(hSession, Attribute::Uint32, (void *) worker) {

			get.asString = [](const Attribute & attr, const void *worker) {
				return std::to_string(attr.getUint32());
			};

			get.asInt32 = [](const Attribute & attr, const void *worker) {
				return (int32_t) attr.getUint32();
			};

			get.asUint32 = [](const Attribute & attr, const void *worker) {

				errno = 0;
				unsigned int value = ((const LIB3270_UINT_PROPERTY *) worker)->get(attr.getTN3270Session());

				if(errno != 0) {
					throw std::system_error(errno, std::system_category());
				}

				return (uint32_t) value;

			};

			get.asBoolean = [](const Attribute & attr, const void *worker) {
				return (attr.getUint32() != 0);
			};

		}

	};

	// String attribute
	class StringAttribute : public Attribute {
	public:
		StringAttribute(H3270 *hSession, const LIB3270_STRING_PROPERTY *worker) : Attribute(hSession, Attribute::String, (void *) worker) {

			get.asString = [](const Attribute & attr, const void *worker) {

				const char * str = ((const LIB3270_STRING_PROPERTY *) worker)->get(attr.getTN3270Session());

				if(str) {
					return string(str);
				}

				throw std::system_error(errno, std::system_category());

			};

			get.asInt32 = [](const Attribute & attr, const void *worker) {

				const char * str = ((const LIB3270_STRING_PROPERTY *) worker)->get(attr.getTN3270Session());

				if(str) {
					return (int32_t) atoi(str);
				}

				throw std::system_error(errno, std::system_category());
			};


		}

	};

	// Boolean attribute
	class BooleanAttribute : public Attribute {
	public:
		BooleanAttribute(H3270 *hSession, const LIB3270_INT_PROPERTY *worker) : Attribute(hSession, Attribute::Boolean, (void *) worker) {

			get.asString = [](const Attribute & attr, const void *worker) {
				return attr.getBoolean() ? "true" : "false";
			};

			get.asInt32 = [](const Attribute & attr, const void *worker) {

				errno = 0;
				int value = ((const LIB3270_INT_PROPERTY *) worker)->get(attr.getTN3270Session());

				if(errno != 0) {
					throw std::system_error(errno, std::system_category());
				}

				return (int32_t) value;

			};

		}

	};

	// Toggle attribute
	class ToggleAttribute : public Attribute {
	public:
		ToggleAttribute(H3270 *hSession, const LIB3270_TOGGLE_ENTRY *worker) : Attribute(hSession, Attribute::Boolean, (void *) worker) {

			get.asString = [](const Attribute & attr, const void *worker) {
				return attr.getBoolean() ? "true" : "false";
			};

			get.asInt32 = [](const Attribute & attr, const void *worker) {

				errno = 0;

				int value = lib3270_get_toggle(attr.getTN3270Session(),((const LIB3270_TOGGLE_ENTRY *) worker)->id);

				if(errno != 0) {
					throw std::system_error(errno, std::system_category());
				}

				return (int32_t) value;

			};

			set.asInt32 = [](const Attribute & attr, const void *worker, const int32_t value) {
				lib3270_set_toggle(attr.getTN3270Session(),((const LIB3270_TOGGLE_ENTRY *) worker)->id, (int) value);
			};

			set.asBoolean = [](const Attribute & attr, const void *worker, const bool value) {
				lib3270_set_toggle(attr.getTN3270Session(),((const LIB3270_TOGGLE_ENTRY *) worker)->id, (int) value);
			};

		}

	};

	Attribute Local::Session::getAttribute(const char *name) const {

		std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);

		// Check for integer properties.
		{
			const LIB3270_INT_PROPERTY * intprop = lib3270_get_int_properties_list();
			for(size_t ix = 0; intprop[ix].name; ix++) {

				if(!strcasecmp(name,intprop[ix].name)) {
					return IntAttribute(hSession,&intprop[ix]);
				}

			}
		}

		// Check for unsigned int properties
		{
			const LIB3270_UINT_PROPERTY * intprop = lib3270_get_unsigned_properties_list();
			for(size_t ix = 0; intprop[ix].name; ix++) {

				if(!strcasecmp(name,intprop[ix].name)) {
					return UnsignedIntAttribute(hSession,&intprop[ix]);
				}

			}

		}

		// Check for string properties
		{
			const LIB3270_STRING_PROPERTY * strprop = lib3270_get_string_properties_list();

			for(size_t ix = 0; strprop[ix].name; ix++) {

				if(!strcasecmp(name,strprop[ix].name)) {
					return StringAttribute(hSession,&strprop[ix]);
				}

			}

		}

		// Check for boolean properties
		{
			const LIB3270_TOGGLE_ENTRY *toggles = lib3270_get_toggle_list();
			for(size_t ix = 0; toggles[ix].name; ix++) {

				if(!strcasecmp(name,toggles[ix].name)) {
					return ToggleAttribute(hSession,&toggles[ix]);
				}

			}

			const LIB3270_INT_PROPERTY * intprop = lib3270_get_boolean_properties_list();
			for(size_t ix = 0; intprop[ix].name; ix++) {

				if(!strcasecmp(name,intprop[ix].name)) {
					return BooleanAttribute(hSession,&intprop[ix]);
				}

			}

		}

		// Not found!
		throw std::runtime_error("Invalid attribute");

	}

	void Local::Session::getAttributes(std::vector<Attribute> attributes) const {

		std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);

		// Add integer properties.
		{
			const LIB3270_INT_PROPERTY * intprop = lib3270_get_int_properties_list();
			for(size_t ix = 0; intprop[ix].name; ix++) {
				attributes.push_back(IntAttribute(hSession,&intprop[ix]));
			}
		}

		// Add unsigned int properties
		{
			const LIB3270_UINT_PROPERTY * intprop = lib3270_get_unsigned_properties_list();
			for(size_t ix = 0; intprop[ix].name; ix++) {
				attributes.push_back(UnsignedIntAttribute(hSession,&intprop[ix]));
			}

		}

		// Add string properties
		{
			const LIB3270_STRING_PROPERTY * strprop = lib3270_get_string_properties_list();

			for(size_t ix = 0; strprop[ix].name; ix++) {
				attributes.push_back(StringAttribute(hSession,&strprop[ix]));
			}

		}

		// Add boolean properties
		{
			const LIB3270_TOGGLE_ENTRY *toggles = lib3270_get_toggle_list();
			for(size_t ix = 0; toggles[ix].name; ix++) {
				attributes.push_back(ToggleAttribute(hSession,&toggles[ix]));
			}

			const LIB3270_INT_PROPERTY * intprop = lib3270_get_boolean_properties_list();
			for(size_t ix = 0; intprop[ix].name; ix++) {
				attributes.push_back(BooleanAttribute(hSession,&intprop[ix]));
			}

		}

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


