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

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

 	std::string	Local::Session::get() const {

		std::lock_guard<std::recursive_mutex> lock(const_cast<Local::Session *>(this)->sync);

		lib3270_ptr<char> text = lib3270_get_string_at_address(hSession, 0, -1, '\n');

        if(!text) {
            throw std::runtime_error( _("Can't get screen contents") );
        }

        return std::string((char *) text);
	}

	std::string	Local::Session::get(int32_t baddr, int32_t len, uint8_t lf) const {

		std::lock_guard<std::recursive_mutex> lock(const_cast<Local::Session *>(this)->sync);

		lib3270_ptr<char> text = lib3270_get_string_at_address(hSession, baddr, len, lf);

        if(!text) {
            throw std::runtime_error( _("Can't get screen contents") );
        }

        return std::string{(char *) text};
	}

	std::string	Local::Session::get(uint32_t row, uint32_t col, int32_t len, uint8_t lf) const {

		std::lock_guard<std::recursive_mutex> lock(const_cast<Local::Session *>(this)->sync);

		lib3270_ptr<char> text = lib3270_get_string_at(hSession, row, col, len, lf);

        if(!text) {
            throw std::runtime_error( _("Can't get screen contents") );
        }

        return std::string((char *) text);

	}

	ProgramMessage Local::Session::getProgramMessage() const {

		std::lock_guard<std::recursive_mutex> lock(const_cast<Local::Session *>(this)->sync);
		return (ProgramMessage) lib3270_get_program_message(this->hSession);

	}

	ConnectionState Local::Session::getConnectionState() const {

		std::lock_guard<std::recursive_mutex> lock(const_cast<Local::Session *>(this)->sync);
		return (ConnectionState) lib3270_get_connection_state(this->hSession);

	}

	SSLState Local::Session::getSSLState() const {

		std::lock_guard<std::recursive_mutex> lock(const_cast<Local::Session *>(this)->sync);
		return (TN3270::SSLState) lib3270_get_ssl_state(hSession);

	}

	LIB3270_KEYBOARD_LOCK_STATE Local::Session::getKeyboardLockState() const {

		std::lock_guard<std::recursive_mutex> lock(const_cast<Local::Session *>(this)->sync);
		return lib3270_get_keyboard_lock_state(hSession);
	}

	unsigned short Local::Session::getScreenWidth() const {
		std::lock_guard<std::recursive_mutex> lock(const_cast<Local::Session *>(this)->sync);
		return (unsigned short) lib3270_get_width(hSession);
	}

	unsigned short Local::Session::getScreenHeight() const {
		std::lock_guard<std::recursive_mutex> lock(const_cast<Local::Session *>(this)->sync);
		return (unsigned short) lib3270_get_height(hSession);
	}

	unsigned short Local::Session::getScreenLength() const {
		std::lock_guard<std::recursive_mutex> lock(const_cast<Local::Session *>(this)->sync);
		return (unsigned short) lib3270_get_length(hSession);
	}

	unsigned short Local::Session::getCursorAddress() {
		std::lock_guard<std::recursive_mutex> lock(sync);

		int rc = lib3270_get_cursor_address(hSession);

		if(!rc)
			chkResponse(errno);

		return rc;
	}

	struct Session::Cursor Local::Session::getCursorPosition() {

		std::lock_guard<std::recursive_mutex> lock(sync);

		unsigned short row = 0, col = 0;

		return Session::Cursor(row,col);

	};

	std::string Local::Session::getVersion() const {

		std::lock_guard<std::recursive_mutex> lock(const_cast<Local::Session *>(this)->sync);
		return lib3270_get_version();

	}

	std::string Local::Session::getRevision() const {

		std::lock_guard<std::recursive_mutex> lock(const_cast<Local::Session *>(this)->sync);
		return lib3270_get_revision();

	}

	std::string Local::Session::getAssociatedLUName() const {

		std::lock_guard<std::recursive_mutex> lock(const_cast<Local::Session *>(this)->sync);
		const char * luname = lib3270_get_associated_luname(hSession);
		return string(luname ? luname : "");

	}

	std::string Local::Session::getHostURL() const {

		std::lock_guard<std::recursive_mutex> lock(const_cast<Local::Session *>(this)->sync);
		return lib3270_get_url(hSession);

	}

 }


