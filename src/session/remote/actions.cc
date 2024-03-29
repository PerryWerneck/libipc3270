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
 #include <lib3270/actions.h>

#ifndef _WIN32
	#include <unistd.h>
#endif // _WIN32

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

 	void Abstract::Session::connect(const char *url, time_t seconds) {

		RequestFactory(Request::Method,"connect")->push(url ? url : "").call();

		if(seconds)
			this->waitForConnectionState(CONNECTED_TN3270E,seconds);

	}

	void Abstract::Session::disconnect() {

		RequestFactory(Request::Method,"disconnect")->call();

	}

	void Abstract::Session::pfkey(unsigned short value) {

		int32_t rc = RequestFactory(Request::Method,"pfkey")->push((int32_t) value).get_int();

		if(rc) {
            throw std::system_error((int) rc, std::system_category());
		}

	}

	void Abstract::Session::pakey(unsigned short value) {

		int32_t rc = RequestFactory(Request::Method,"pakey")->push((int32_t) value).get_int();

		if(rc) {
            throw std::system_error((int) rc, std::system_category());
		}

	}

	void Abstract::Session::print(LIB3270_CONTENT_OPTION GNUC_UNUSED(option)) {
		throw std::system_error(ENOTSUP, std::system_category());
	}

	void Abstract::Session::push(const KeyboardAction action) {
		this->ActionFactory(toCharString(action))->activate();
	}


 }


