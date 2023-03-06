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
		return get(0,-1,'\n');
	}

	std::string	Local::Session::get(int32_t baddr, int32_t len, uint8_t lf) const {

		return handler->get<std::string>([baddr,len,lf](H3270 * hSession){

			lib3270_ptr<char> text = lib3270_get_string_at_address(hSession, baddr, len, lf);

			if(!text) {
				throw std::runtime_error( _("Can't get screen contents") );
			}

			return std::string{(char *) text};

		});

	}

	std::string	Local::Session::get(uint32_t row, uint32_t col, int32_t len, uint8_t lf) const {

		return handler->get<std::string>([row,col,len,lf](H3270 * hSession){

			lib3270_ptr<char> text = lib3270_get_string_at(hSession, row, col, len, lf);

			if(!text) {
				throw std::runtime_error( _("Can't get screen contents") );
			}

			return std::string{(char *) text};

		});

	}

	ProgramMessage Local::Session::getProgramMessage() const {

		return handler->get<ProgramMessage>([](H3270 * hSession){
			return (ProgramMessage) lib3270_get_program_message(hSession);
		});

	}

	ConnectionState Local::Session::getConnectionState() const {

		return handler->get<ConnectionState>([](H3270 * hSession){
			return (ConnectionState) lib3270_get_connection_state(hSession);
		});

	}

	SSLState Local::Session::getSSLState() const {

		return handler->get<SSLState>([](H3270 * hSession){
			return (SSLState) lib3270_get_ssl_state(hSession);
		});

	}

	LIB3270_KEYBOARD_LOCK_STATE Local::Session::getKeyboardLockState() const {

		return handler->get<LIB3270_KEYBOARD_LOCK_STATE>([](H3270 * hSession){
			return lib3270_get_keyboard_lock_state(hSession);
		});

	}

	bool Local::Session::connected() const {

		return handler->get<bool>([](H3270 * hSession){
			return lib3270_is_connected(hSession);
		});

	}

	bool Local::Session::ready() const {

		return handler->get<bool>([](H3270 * hSession){
			return lib3270_is_ready(hSession);
		});

	}

	unsigned short Local::Session::getScreenWidth() const {

		return handler->get<unsigned short>([](H3270 * hSession){
			return lib3270_get_width(hSession);
		});

	}

	unsigned short Local::Session::getScreenHeight() const {

		return handler->get<unsigned short>([](H3270 * hSession){
			return lib3270_get_height(hSession);
		});

	}

	unsigned short Local::Session::getScreenLength() const {

		return handler->get<unsigned short>([](H3270 * hSession){
			return lib3270_get_length(hSession);
		});

	}

	unsigned short Local::Session::getCursorAddress() const {

		return handler->get<unsigned short>([this](H3270 * hSession){

			int rc = lib3270_get_cursor_address(hSession);

			if(!rc)
				handler->chkResponse(errno);

			return rc;

		});

	}

	struct Session::Cursor Local::Session::getCursorPosition() {

		unsigned short row = 0, col = 0;
		handler->call([&row,&col](H3270 * hSession){
			return lib3270_get_cursor_position(hSession,&row,&col);
		});
		return Session::Cursor(row,col);

	};

	std::string Local::Session::getVersion() const {

		return handler->get<std::string>([](H3270 *){
			return lib3270_get_version();
		});

	}

	std::string Local::Session::getRevision() const {

		return handler->get<std::string>([](H3270 *){
			return lib3270_get_revision();
		});

	}

	std::string Local::Session::getAssociatedLUName() const {

		return handler->get<std::string>([](H3270 * hSession){
			const char * luname = lib3270_get_associated_luname(hSession);
			return string{luname ? luname : ""};
		});

	}

	std::string Local::Session::getHostURL() const {

		return handler->get<std::string>([](H3270 * hSession){
			return lib3270_get_url(hSession);
		});

	}

 }


