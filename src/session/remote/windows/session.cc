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
 * @file src/os/windows/session.cc
 *
 * @brief Implements WIN32 remote session methods.
 *
 * @author perry.werneck@gmail.com
 *
 */

 #ifdef HAVE_CONFIG_H
	#include <config.h>
 #endif // HAVE_CONFIG_H

 #include <private/session.h>
 #include "pipe-request.h"
 #include <lib3270/ipc/session.h>
 #include <lib3270/ipc/action.h>
 #include <algorithm>
 #include <ipc-client-internals.h>

 /*
 #include "../private.h"
 #include <ipc-client-internals.h>
 #include <cstring>
 #include <lib3270/trace.h>
 #include <algorithm>
 */

 using namespace std;

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

 	std::shared_ptr<Abstract::Session> Abstract::Session::getRemoteInstance(const char *id, const char *charset) {

		if(!(id && *id)) {
			throw std::system_error(EINVAL, std::system_category(),"Empty session id");
		}

		string pipename{"\\\\.\\pipe\\"};
		const char *ptr = id;

		if(id[0] == ':') {

#ifdef PRODUCT_NAME
			pipename += PRODUCT_NAME;
#else
			pipename += "pw3270";
#endif // PRODUCT_NAME

			if(!id[1]) {
				throw std::system_error(EINVAL, std::system_category(),string{"Invalid remote session id '"} + id + "'");
			}

		} else {

			ptr = strchr(id,':');

			if(!ptr) {
				throw std::system_error(EINVAL, std::system_category(),string{"Invalid remote session id '"} + id + "'");
			}

			pipename += string{id, ((size_t) (ptr - id))};

		}

		pipename += "\\";
		pipename += (ptr+1);

		std::transform(pipename.begin(), pipename.end(), pipename.begin(), ::tolower);

		debug("id: \"", id, "\" pipename: \"", pipename , "\"");

		HANDLE hPipe = CreateFile(
			TEXT(pipename.c_str()),		// pipe name
			GENERIC_READ |  			// read and write access
			GENERIC_WRITE,
			0,              			// no sharing
			NULL,           			// default security attributes
			OPEN_EXISTING,  			// opens existing pipe
			0,              			// default attributes
			NULL						// no template file
		);

		if (hPipe == INVALID_HANDLE_VALUE) {
			string msg = "Can't open IPC Channel \"";
			msg += pipename;
			msg += "\"";
			throw std::runtime_error(msg);
		}

		// The pipe connected; change to message-read mode.
		DWORD dwMode = PIPE_READMODE_MESSAGE;
		if(!SetNamedPipeHandleState(hPipe,&dwMode,NULL,NULL)) {
			throw std::runtime_error("Can't set IPC Channel mode");
		}

		class Action : public TN3270::Action {
		private:
			std::shared_ptr<Pipe::Handler> handler;

		public:
			Action(std::shared_ptr<Pipe::Handler> h, const LIB3270_ACTION *dsc) : TN3270::Action{dsc}, handler{h} {
			}

			bool activatable() const override {
				bool rc;
				Pipe::Request{handler,Request::Method,"activatable"}.push(name()).call().pop(rc);
				return rc;
			}

			void activate() override {
				debug("Calling remote action '",name(),"'");
				int32_t rc = Pipe::Request{handler,Request::Method,"action"}.push(name()).get_int();
				debug("Remote action '",name(),"' has returned ",rc);
				if(rc) {
					throw std::system_error((int) rc, std::system_category());
				}
			}

			void wait(time_t seconds) override {

				time_t end = time(nullptr) + seconds;
				while(time(nullptr) < end) {
					int32_t rc = Pipe::Request{handler,Request::Method,"waitForReady"}.push((uint32_t) 1).get_int();
					if(!rc) {
						break;
					} else if(rc != ETIMEDOUT) {
						throw std::system_error((int) rc, std::system_category());
					}
				}

			}


		};

		class Session : public Abstract::Session {
		private:
			std::shared_ptr<Pipe::Handler> handler;

		public:
			Session(HANDLE h) : Abstract::Session{}, handler{std::make_shared<Pipe::Handler>(h)} {
			}

			std::shared_ptr<TN3270::Request> RequestFactory(const Request::Type type, const char *name) const override {
				return make_shared<Pipe::Request>(handler,type,name);
			}

			std::shared_ptr<TN3270::Action> ActionFactory(const LIB3270_ACTION *descriptor) {
				return make_shared<Action>(handler,descriptor);
			}

			std::shared_ptr<Field> FieldFactory(int baddr) override {
				throw std::system_error(ENOTSUP, std::system_category());
			}

			std::shared_ptr<Field> FieldFactory(unsigned short row, unsigned short col) override {
				throw std::system_error(ENOTSUP, std::system_category());
			}

		};

		return make_shared<Session>(hPipe);

 	}


 }


