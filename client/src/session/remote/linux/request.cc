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
 * @brief Implements linux request constructors based on TN3270::IPC::Session.
 *
 * @author perry.werneck@gmail.com
 *
 */

  #include "../private.h"
  #include <iostream>

  using std::clog;
  using std::endl;
  using std::string;

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	IPC::Request::Request(const IPC::Session &session, const char *method) : Request(session.conn) {

		request.msg = dbus_message_new_method_call(
							session.name.c_str(),					// Destination
							session.path.c_str(),					// Path
							session.interface.c_str(),				// Interface
							method									// Method
						);

		if(!request.msg) {
			throw std::runtime_error("Can't create D-Bus Method Call");
		}

		dbus_message_iter_init_append(request.msg, &request.iter);

	}

	IPC::Request::Request(const IPC::Session &session, bool isSet, const char *property) : Request(session.conn) {

		request.msg = dbus_message_new_method_call(
							session.name.c_str(),					// Destination
							session.path.c_str(),					// Path
							"org.freedesktop.DBus.Properties",		// Interface
							(isSet ? "Set" : "Get")
						);

		if(!request.msg) {
			throw std::runtime_error("Can't create D-Bus Property Call");
		}

		dbus_message_iter_init_append(request.msg, &request.iter);

		//
		// https://dbus.freedesktop.org/doc/dbus-specification.html#standard-interfaces-properties
		// org.freedesktop.DBus.Properties.Get (in STRING interface_name,
		// 								   in STRING property_name,
		// 								   out VARIANT value);
		// org.freedesktop.DBus.Properties.Set (in STRING interface_name,
		// 								   in STRING property_name,
		//
		const char *interface_name = session.interface.c_str();

		push(DBUS_TYPE_STRING,&interface_name);
		push(DBUS_TYPE_STRING,&property);

		// Set property argument should be variant!
		this->request.variant = isSet;

	}

 }


