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
 * @file src/os/linux/linux/session.cc
 *
 * @brief Implements Linux session methods.
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include <config.h>
 #include <private/session.h>
 #include <dbus/dbus.h>
 #include <string>
 #include <mutex>
 #include "dbus-request.h"
 #include <lib3270/ipc/action.h>
 #include <ipc-client-internals.h>

 using namespace std;

 namespace TN3270 {

	std::shared_ptr<Abstract::Session> Abstract::Session::getRemoteInstance(const char *id, const char *charset) {

		// Get D-Bus Session.
		DBusError err;

		dbus_error_init(&err);
		DBusConnection *connection = dbus_bus_get(DBUS_BUS_SESSION, &err);
		if (dbus_error_is_set(&err)) {
			std::string message(err.message);
			dbus_error_free(&err);
			throw std::runtime_error(message);
		}

		if(!connection) {
			throw runtime_error("Unexpected error getting session bus");
		}

		class Action : public TN3270::Action {
		private:
			DBusConnection *connection;
			string id;

		public:
			Action(DBusConnection *c, const std::string &i, const LIB3270_ACTION *dsc) : TN3270::Action{dsc}, connection{c}, id{i} {
			}

			bool activatable() const override {
				bool rc;
				DBus::Request{connection,id,Request::Method,"activatable"}.push(name()).call().pop(rc);
				return rc;
			}

			void activate() override {
				debug("Calling remote action '",name(),"'");
				int32_t rc = DBus::Request{connection,id,Request::Method,"action"}.push(name()).get_int();
				debug("Remote action '",name(),"' has returned ",rc);
				if(rc) {
					throw std::system_error((int) rc, std::system_category());
				}
			}

		};

		class Session : public Abstract::Session {
		private:
			DBusConnection *connection;
			string id;

		public:
			Session(DBusConnection *c, const char *i, const char *charset) : Abstract::Session{}, connection{c}, id{i} {
				if(charset && *charset) {
					setCharSet(charset);
				}
			}

			std::shared_ptr<Request> RequestFactory(const Request::Type type, const char *name) const override {
				return make_shared<DBus::Request>(connection,id.c_str(),type,name);
			}

			std::shared_ptr<TN3270::Action> ActionFactory(const LIB3270_ACTION *descriptor) {
				return make_shared<Action>(connection,id,descriptor);
			}

			std::shared_ptr<Field> FieldFactory() override {
				throw std::system_error(ENOTSUP, std::system_category());
			}

			std::shared_ptr<Field> FieldFactory(int) override {
				throw std::system_error(ENOTSUP, std::system_category());
			}

			std::shared_ptr<Field> FieldFactory(unsigned short, unsigned short) override {
				throw std::system_error(ENOTSUP, std::system_category());
			}

		};

		return make_shared<Session>(connection,id,charset);

	}

 }

