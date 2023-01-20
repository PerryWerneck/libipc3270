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

 #include <config.h>
 #include <lib3270/ipc/request.h>
 #include <private/session.h>

 using namespace std;

 namespace TN3270 {

	std::string	Abstract::Session::get() const {
		return RequestFactory(Request::Method,"getString")->get_string();
	}

	std::string	Abstract::Session::get(int32_t baddr, int32_t len, uint8_t lf) const {
		return RequestFactory(Request::Method,"getStringAtAddress")->push(baddr,len,lf).get_string();
	}

	std::string	Abstract::Session::get(uint32_t row, uint32_t col, int32_t len, uint8_t lf) const {
		return RequestFactory(Request::Method,"getStringAt")->push(row,col,len,lf).get_string();
	}

	ProgramMessage Abstract::Session::getProgramMessage() const {
		return getProperty<uint32_t,ProgramMessage>("program_message");
	}

	ConnectionState Abstract::Session::getConnectionState() const {
		return getProperty<uint32_t,ConnectionState>("cstate");
	}

	SSLState Abstract::Session::getSSLState() const {
		return getProperty<int32_t,SSLState>("sslstate");
	}

	LIB3270_KEYBOARD_LOCK_STATE Abstract::Session::getKeyboardLockState() const {
		return getProperty<uint32_t,LIB3270_KEYBOARD_LOCK_STATE>("kybdlock");
	}

	Session::Cursor Abstract::Session::getCursorPosition() {

		uint32_t row = 0, col = 0;

		RequestFactory(Request::Method,"getCursorPosition")->call().pop(row,col);

		return Session::Cursor(row,col);

	}

	unsigned short Abstract::Session::getScreenWidth() const {
		return getProperty<uint32_t,unsigned short>("width");
	}

	unsigned short Abstract::Session::getScreenHeight() const {
		return getProperty<uint32_t,unsigned short>("height");
	}

	unsigned short Abstract::Session::getScreenLength() const {
		return getProperty<uint32_t,unsigned short>("length");
	}


 }


