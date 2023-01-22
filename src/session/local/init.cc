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
 * @file src/session/local/init.cc
 *
 * @brief Implement lib3270 direct access layout (NO IPC).
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include "private.h"

#ifdef _WIN32
 #include <lmcons.h>
#endif // _WIN32

#if defined(_MSC_VER)
	#pragma comment(lib,LIB3270_STRINGIZE_VALUE_OF(LIB3270_NAME)".lib")
#endif // _MSC_VER

 extern "C" {
	 #include <lib3270/session.h>
 }


/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	void Local::Session::Handler::chkresponse(int rc) {
	}

	void Local::Session::Handler::call(const std::function<int(H3270 * hSession)> &method) {
		std::lock_guard<std::mutex> lock(handler);
		TN3270::chkResponse(method(hSession));
	}

 	Local::Session::Session(const char *charset) : Abstract::Session(), handler{std::make_shared<Handler>()} {

		std::lock_guard<std::mutex> lock(*handler);

		this->timeout = 5;

		lib3270_set_user_data(handler->hSession,(void *) this);

		this->setCharSet(charset);

		lib3270_set_popup_handler(handler->hSession, &popupHandler);

		// Setup callbacks
		struct lib3270_session_callbacks *cbk;

		cbk = lib3270_get_session_callbacks(this->hSession,LIB3270_STRINGIZE_VALUE_OF(LIB3270_REVISION),sizeof(struct lib3270_session_callbacks));
		if(!cbk) {

			string message(_("Invalid callback table; "));
			message += "lib";
			message += LIB3270_STRINGIZE_VALUE_OF(LIB3270_NAME);

			if(strcasecmp(LIB3270_STRINGIZE_VALUE_OF(LIB3270_REVISION),lib3270_get_revision())) {
				 message += _(" is not in the required revision " );
				 message += LIB3270_STRINGIZE_VALUE_OF(LIB3270_REVISION);
				 message += ".";
			} else {
				message += _(" is invalid.");
			}

			throw runtime_error(message);

		}

		cbk->update_connect	= connectHandler;

	}

	Local::Session::~Session() {
	}

 }


