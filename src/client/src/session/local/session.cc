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
 * @file src/session/local/session.cc
 *
 * @brief Implement lib3270 direct access layout (NO IPC).
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include "../private.h"
 #include <lib3270/actions.h>
 #include <lib3270/properties.h>
 #include <cstring>

 extern "C" {
	 #include <lib3270/session.h>
 }

 using std::string;

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	Local::Session::Session() : Abstract::Session() {

		std::lock_guard<std::mutex> lock(sync);

		this->hSession = lib3270_session_new("");
		lib3270_set_user_data(this->hSession,(void *) this);
		setCharSet(lib3270_get_display_charset(this->hSession));

		lib3270_set_popup_handler(this->hSession, popupHandler);

		// Setup callbacks
		struct lib3270_session_callbacks *cbk;

		cbk = lib3270_get_session_callbacks(this->hSession,sizeof(struct lib3270_session_callbacks));
		if(!cbk) {
			throw std::runtime_error( _("Invalid callback table, possible version mismatch in lib3270") );
		}

		cbk->update_connect	= connectHandler;


	}

	Local::Session::~Session() {

		std::lock_guard<std::mutex> lock(sync);

		lib3270_session_free(this->hSession);
		this->hSession = nullptr;
	}

	void Local::Session::wait(time_t timeout) {

		std::lock_guard<std::mutex> lock(sync);

		int rc = lib3270_wait_for_ready(this->hSession, timeout);

		if(rc) {
			throw std::system_error(rc, std::system_category());
		}

	}

	void Local::Session::connect(const char *url) {
		std::lock_guard<std::mutex> lock(sync);
		int rc = lib3270_connect_url(hSession,url,0);

		if(rc) {
            throw std::system_error(rc, std::system_category());
		}

    }

	void Local::Session::disconnect() {
		std::lock_guard<std::mutex> lock(sync);
		lib3270_disconnect(hSession);
	}

	// Wait for session state.
	void Local::Session::waitForReady(time_t timeout) throw() {
		this->wait(timeout);
	}

	std::string	Local::Session::toString(int baddr, size_t len, char lf) const {

		std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);

        char * text = lib3270_get_string_at_address(hSession, baddr, len, lf);

        if(!text) {
            throw std::runtime_error( _("Can't get screen contents") );
        }

        string rc = convertFromHost(text);

        lib3270_free(text);

		return rc;

	}

	std::string	Local::Session::toString(int row, int col, size_t sz, char lf) const {

		std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);

        char * text = lib3270_get_string_at(hSession, row, col, sz, lf);

        if(!text) {
            throw std::runtime_error( _("Can't get screen contents") );
        }

        string rc = convertFromHost(text);

        lib3270_free(text);

		return rc;
	}

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

	ProgramMessage Local::Session::getProgramMessage() const {
		std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);
		return (ProgramMessage) lib3270_get_program_message(this->hSession);
	}

	ConnectionState Local::Session::getConnectionState() const {
		std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);
		return (ConnectionState) lib3270_get_connection_state(this->hSession);
	}

	/// @brief Set field at current position, jumps to next writable field.
	TN3270::Session & Local::Session::push(const char *text) {
		std::lock_guard<std::mutex> lock(sync);
		return *this;
	}

	TN3270::Session & Local::Session::push(int baddr, const std::string &text) {
		std::lock_guard<std::mutex> lock(sync);
		return *this;
	}

	TN3270::Session & Local::Session::push(int row, int col, const std::string &text) {
		std::lock_guard<std::mutex> lock(sync);
		return *this;
	}

	TN3270::Session & Local::Session::push(const PFKey key) {
		std::lock_guard<std::mutex> lock(sync);
		lib3270_pfkey(hSession,(int) key);
		return *this;
	}

	TN3270::Session & Local::Session::push(const PAKey key) {
		std::lock_guard<std::mutex> lock(sync);
		lib3270_pakey(hSession,(int) key);
		return *this;
	}

	TN3270::Session & Local::Session::push(const Action action) {

		typedef int (*ActionCallback)(H3270 *);

		static const ActionCallback actions[] = {
            lib3270_enter,
            lib3270_erase,
            lib3270_eraseeof,
            lib3270_eraseeol,
            lib3270_eraseinput
		};

		if( ((size_t) action) > (sizeof(actions)/sizeof(actions[0]))) {
            throw std::system_error(EINVAL, std::system_category());
		}

		std::lock_guard<std::mutex> lock(sync);

		int rc = actions[(size_t) action](hSession);

		if(rc) {
			throw std::system_error(errno, std::system_category());
		}

		return *this;
	}

	TN3270::Session & Local::Session::pop(int baddr, std::string &text) {

		std::lock_guard<std::mutex> lock(sync);

		if(!lib3270_is_connected(hSession)) {
			throw std::system_error(ENOTCONN, std::system_category());
		}

		char *contents = lib3270_get_field_text_at(hSession, baddr);

		if(!contents) {
			throw std::system_error(errno, std::system_category());
		}

		text.assign(convertFromHost(contents).c_str());

		lib3270_free(contents);

		return *this;
	}

	TN3270::Session & Local::Session::pop(int row, int col, std::string &text) {
		return this->pop(lib3270_translate_to_address(hSession,row,col),text);
	}

	TN3270::Session & Local::Session::pop(std::string &text) {

		std::lock_guard<std::mutex> lock(sync);

		if(!lib3270_is_connected(hSession)) {
			throw std::system_error(ENOTCONN, std::system_category());
		}

		int baddr = lib3270_get_cursor_address(hSession);
		if(baddr < 0) {
			throw std::system_error(errno, std::system_category());
		}

		char *contents = lib3270_get_field_text_at(hSession, baddr);

		if(!contents) {
			throw std::system_error(errno, std::system_category());
		}

		text.assign(convertFromHost(contents).c_str());

		lib3270_free(contents);

		baddr = lib3270_get_next_unprotected(hSession,baddr);
		if(!baddr) {
			baddr = lib3270_get_next_unprotected(hSession,0);
		}

		if(lib3270_set_cursor_address(hSession,baddr)) {
			throw std::system_error(errno, std::system_category());
		}

		return *this;
	}

	/// @brief Set cursor address.
	///
	/// @param addr	Cursor address.
	TN3270::Session & Local::Session::setCursorPosition(unsigned short addr) {

		if(lib3270_set_cursor_address(hSession,addr) < 0) {
			throw std::system_error(errno, std::system_category());
		}

		return *this;
	}

	/// @brief Set cursor position.
	///
	/// @param row	New cursor row.
	/// @param col	New cursor column.
	TN3270::Session & Local::Session::setCursorPosition(unsigned short row, unsigned short col) {

		if(lib3270_set_cursor_position(hSession,row,col)) {
			throw std::system_error(errno, std::system_category());
		}

		return *this;

	}

	// Get properties.
	std::string Local::Session::getVersion() const {
		return lib3270_get_version();
	}

	std::string Local::Session::getRevision() const {
		return lib3270_get_revision();
	}

	/// @brief Execute action by name.
	TN3270::Session & Local::Session::action(const char *action_name) {

		if(lib3270_action(hSession,action_name)) {
			throw std::system_error(errno, std::system_category());
		}

		return *this;
	}

 }

