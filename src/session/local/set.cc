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

 	void Local::Session::set(const std::string &str) {

		handler->call([str](H3270 * hSession){
			int rc = lib3270_set_field(hSession,str.c_str(),str.length());
			if(rc < 0)
				return -rc;
			return 0;
		});

	}

	void Local::Session::set(int32_t baddr, const std::string &str) {

		handler->call([baddr,str](H3270 * hSession){
			int rc = lib3270_set_string_at_address(hSession,baddr,(unsigned char *) str.c_str(),str.length());
			if(rc < 0)
				return -rc;
			return 0;
		});

	}

	void Local::Session::set(uint32_t row, uint32_t col, const std::string &str) {

		handler->call([row,col,str](H3270 * hSession){
			int rc = lib3270_set_string_at(hSession,row,col,(unsigned char *) str.c_str(),str.length());
			if(rc < 0)
				return -rc;
			return 0;
		});

	}

	void Local::Session::setCharSet(const char *charset) {

		handler->call([this,charset](H3270 * hSession){
			Abstract::Session::setCharSet(lib3270_get_display_charset(hSession),charset);
			return 0;
		});

	}

	void Local::Session::setUnlockDelay(unsigned short delay) {

		handler->call([delay](H3270 * hSession){
			return lib3270_set_unlock_delay(hSession,delay);
		});

	}

	void Local::Session::setTimeout(time_t timeout) {
		this->timeout = timeout;
	}

	void Local::Session::setLockOnOperatorError(bool lock) {

		handler->call([lock](H3270 * hSession){
			return lib3270_set_lock_on_operator_error(hSession,lock ? 1 : 0);
		});

	}

	unsigned short Local::Session::setCursor(int addr) {

		return handler->get<unsigned short>([this,addr](H3270 * hSession){

			int rc = lib3270_set_cursor_address(hSession,addr);
			if(rc < 0)
				handler->chkResponse(-rc);

			return (unsigned short) rc;

		});

	}

	unsigned short Local::Session::setCursor(unsigned short row, unsigned short col) {

		return handler->get<unsigned short>([this,row,col](H3270 * hSession){

			int rc = lib3270_set_cursor_position(hSession,row,col);
			if(rc < 0)
				handler->chkResponse(-rc);

			return (unsigned short) rc;

		});

	}

	void Local::Session::setHostURL(const char *url) {

		handler->call([url](H3270 * hSession){
			return lib3270_set_url(hSession, url);
		});

	}

 }


