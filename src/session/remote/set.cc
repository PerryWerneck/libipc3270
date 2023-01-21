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
 #include <lib3270/ipc/request.h>
 #include <private/session.h>

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

 	void Abstract::Session::set(const std::string &str) {

		int32_t rc = RequestFactory(Request::Method,"setField")->push(str).get_int();

		if(rc < 0)
			chkResponse(-rc);

	}

	void Abstract::Session::set(int32_t baddr, const std::string &str) {

		int32_t rc = RequestFactory(Request::Method,"setField")->push(baddr,str).get_int();

		if(rc < 0)
			chkResponse(-rc);

	}

	void Abstract::Session::set(uint32_t row, uint32_t col, const std::string &str) {

		int32_t rc = RequestFactory(Request::Method,"setStringAt")->push(row,col,str).get_int();

		if(rc < 0)
			chkResponse(-rc);

	}

	void Abstract::Session::setCharSet(const char *charset) {
		// D-Bus calls are always UTF-8
		Abstract::Session::setCharSet("UTF-8",charset);
	}

	unsigned short Abstract::Session::setCursor(unsigned short row, unsigned short col) {

		int32_t rc = RequestFactory(Request::Method,"setCursorPosition")->push(row,col).get_int();

		if(rc < 0)
			chkResponse(-rc);

		return (unsigned short) rc;

	}

	void Abstract::Session::setHostURL(const char *url) {

		RequestFactory(Request::SetProperty,"url")->push(url).call();

	}

 }


