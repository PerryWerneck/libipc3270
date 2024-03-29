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

 #ifdef HAVE_CONFIG_H
	#include <config.h>
 #endif // HAVE_CONFIG_H

 #include "private.h"
 #include <memory>
 #include <lib3270/ipc/session.h>

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

	std::shared_ptr<Session> Session::getInstance(H3270 *hSession) {

		class Session : public Local::Session {
		private:
			class Handler : public Local::Session::Handler  {
			public:
				Handler(H3270 *h) {
					hSession = h;
				}

				virtual ~Handler() {
				}
			};

		public:
			Session(H3270 *hSession) : Local::Session(std::make_shared<Handler>(hSession)) {
			}

			virtual ~Session() {
			}

		};

		return std::make_shared<Session>(hSession);

	}

	std::shared_ptr<Abstract::Session> Abstract::Session::getLocalInstance(const char *charset) {

		class Session : public Local::Session {
		private:
			class Handler : public Local::Session::Handler  {
			public:
				Handler() {
					std::lock_guard<std::mutex> lock(*this);
					hSession = lib3270_session_new("");
				}

				virtual ~Handler() {
					std::lock_guard<std::mutex> lock(*this);
					lib3270_session_free(hSession);
				}
			};

		public:
			Session(const char *charset) : Local::Session(std::make_shared<Handler>()) {

				// Initialize lib3270 session.
				handler->call([this,charset](H3270 * hSession){

					lib3270_set_user_data(hSession,(void *) this);

					Abstract::Session::setCharSet(lib3270_get_display_charset(hSession),charset);

					lib3270_set_popup_handler(hSession, &popupHandler);

					// Setup callbacks
					struct lib3270_session_callbacks *cbk;

					cbk = lib3270_get_session_callbacks(hSession,LIB3270_STRINGIZE_VALUE_OF(LIB3270_REVISION),sizeof(struct lib3270_session_callbacks));
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

					return 0;

				});

			}

			virtual ~Session() {
			}

		};

		return std::make_shared<Session>(charset);
	}

	void Local::Session::Handler::chkResponse(int rc) {
		TN3270::chkResponse(rc);
	}

	void Local::Session::Handler::call(const std::function<int(H3270 * hSession)> &method) {
		std::lock_guard<std::mutex> lock(*this);
		chkResponse(method(hSession));
	}

 	Local::Session::Session(std::shared_ptr<Handler> h) : Abstract::Session(), handler{h} {
		this->timeout = 5;
	}

	Local::Session::~Session() {
	}

 }


