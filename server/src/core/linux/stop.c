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
 * Referências:
 *
 * https://github.com/joprietoe/gdbus/blob/master/gdbus-example-server.c
 * https://github.com/bratsche/glib/blob/master/gio/tests/gdbus-example-export.c
 *
 * Contatos:
 *
 * perry.werneck@gmail.com	(Alexandre Perry de Souza Werneck)
 * erico.mendonca@gmail.com	(Erico Mascarenhas Mendonça)
 *
 */

#include "gobject.h"
#include <lib3270.h>
#include <ipc-glib.h>

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>

void ipc3270_release_object(ipc3270 *object) {

	if(object->dbus.id) {
		g_dbus_connection_unregister_object(object->dbus.connection,object->dbus.id);
	}

	if(object->dbus.name) {

		debug("Releasing %s",object->dbus.name);

		// https://dbus.freedesktop.org/doc/dbus-specification.html
		GError *err = NULL;

		GVariant * response =
			g_dbus_connection_call_sync (
					object->dbus.connection,
					DBUS_SERVICE_DBUS,
					DBUS_PATH_DBUS,
					DBUS_INTERFACE_DBUS,
					"ReleaseName",
					g_variant_new ("(s)", object->dbus.name),
					NULL,
					G_DBUS_CALL_FLAGS_NONE,
					-1,
					NULL,
					&err
			);

		if(err) {

			g_message("Can't release \"%s\": %s",object->dbus.name,err->message);
			g_error_free(err);
			err = NULL;

		} else if(response) {

			guint32 reply = 0;
			g_variant_get(response, "(u)", &reply);
			g_variant_unref(response);

			switch(reply)
			{
			case DBUS_RELEASE_NAME_REPLY_RELEASED:
				// The caller has released his claim on the given name.
				// Either the caller was the primary owner of the name, and the name is
				// now unused or taken by somebody waiting in the queue for the name,
				// or the caller was waiting in the queue for the name and has now been removed from the queue.
				g_message("%s released",object->dbus.name);
				break;

			case DBUS_RELEASE_NAME_REPLY_NON_EXISTENT:
				// The given name does not exist on this bus.
				g_message("%s does not exist on this bus",object->dbus.name);
				break;

			case DBUS_RELEASE_NAME_REPLY_NOT_OWNER:
				// The caller was not the primary owner of this name, and was also not waiting in the queue to own this name.
				g_message("%s does not exist on this bus", object->dbus.name);
				break;

			default:
				g_message("Unexpected response %u when removing %s",(unsigned int) reply, object->dbus.name);
			}
		}

		g_free(object->dbus.name);
		object->dbus.name = NULL;
	}

}
