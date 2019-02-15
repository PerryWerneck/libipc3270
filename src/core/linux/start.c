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
 * Este programa está nomeado como main.c e possui - linhas de código.
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
#include <lib3270/ipc.h>

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>

static void
	method_call (
		G_GNUC_UNUSED GDBusConnection       *connection,
		G_GNUC_UNUSED const gchar           *sender,
		G_GNUC_UNUSED const gchar           *object_path,
		G_GNUC_UNUSED const gchar           *interface_name,
		const gchar           *method_name,
		GVariant              *parameters,
		GDBusMethodInvocation *invocation,
		gpointer               user_data) {

	g_autoptr (GError) error = NULL;

	GVariant * rc = ipc3270_method_call(G_OBJECT(user_data), method_name, parameters, &error);

	if(error) {

		if(rc) {
			g_variant_unref(rc);
		}

		g_dbus_method_invocation_return_gerror(invocation, error);

	} else if(rc) {

		g_dbus_method_invocation_return_value(invocation, rc);

	} else {

		g_dbus_method_invocation_return_error(invocation, G_DBUS_ERROR, G_DBUS_ERROR_UNKNOWN_METHOD, "Invalid or unexpected method call");

	}


}

static GVariant *
	get_property (
		G_GNUC_UNUSED  GDBusConnection  *connection,
		G_GNUC_UNUSED  const gchar      *sender,
		G_GNUC_UNUSED  const gchar      *object_path,
		G_GNUC_UNUSED  const gchar      *interface_name,
		const gchar      *property_name,
		GError          **error,
		gpointer          user_data)
{

	return ipc3270_get_property(G_OBJECT(user_data), property_name, error);

}

static gboolean
	set_property (
		G_GNUC_UNUSED GDBusConnection  *connection,
		G_GNUC_UNUSED const gchar      *sender,
		G_GNUC_UNUSED const gchar      *object_path,
		G_GNUC_UNUSED const gchar      *interface_name,
		const gchar      *property_name,
		GVariant         *value,
		GError          **error,
		gpointer          user_data)
{

	return ipc3270_set_property(G_OBJECT(user_data), property_name, value, error);

}

void ipc3270_export_object(GObject *object, const char *name, GError **error) {

	char id;

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
	static const GDBusInterfaceVTable interface_vtable = {
		method_call,
		get_property,
		set_property
	};
	#pragma GCC diagnostic pop

	ipc3270 * ipc = IPC3270(object);

	ipc->dbus.connection = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, error);
	if(*error) {
		g_message("Can't get session bus: %s",(*error)->message);
		return;
	}

	g_dbus_connection_set_exit_on_close(ipc->dbus.connection,FALSE);

	for(id='a'; id < 'z' && !ipc->dbus.id && !*error; id++) {

		g_autofree gchar *object_name = g_strdup_printf(PW3270_IPC_SESSION_BUS_NAME,name,id);

		debug("Requesting \"%s\"",object_name);

		// https://dbus.freedesktop.org/doc/dbus-specification.html
		GError *err = NULL;

		GVariant * response =
			g_dbus_connection_call_sync (
					ipc->dbus.connection,
					DBUS_SERVICE_DBUS,
					DBUS_PATH_DBUS,
					DBUS_INTERFACE_DBUS,
					"RequestName",
					g_variant_new ("(su)", object_name, DBUS_NAME_FLAG_DO_NOT_QUEUE),
					NULL,
					G_DBUS_CALL_FLAGS_NONE,
					-1,
					NULL,
					&err
			);

		if(err) {

			g_message("Can't request \"%s\": %s",object_name,err->message);
			g_error_free(err);
			err = NULL;

		} else if(response) {

			guint32 reply = 0;
			g_variant_get(response, "(u)", &reply);
			g_variant_unref(response);

			if(reply == DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {

				ipc->dbus.name = g_strdup(object_name);
				g_message("Got %s", ipc->dbus.name);

				lib3270_set_session_id(ipc->hSession, id);

				// Introspection data for the service we are exporting
				GString * introspection = g_string_new("<node><interface name='" PW3270_IPC_SESSION_INTERFACE_NAME "'>");
				ipc3270_add_terminal_introspection(introspection);
				g_string_append(introspection,"</interface></node>");

				gchar * introspection_xml = g_string_free(introspection,FALSE);

				// debug("\n%s\n",introspection_xml);

				GDBusNodeInfo * introspection_data = g_dbus_node_info_new_for_xml(introspection_xml, NULL);

				// Register object-id
				ipc->dbus.id = g_dbus_connection_register_object (
									ipc->dbus.connection,
									PW3270_IPC_SESSION_OBJECT_PATH,
									introspection_data->interfaces[0],
									&interface_vtable,
									ipc,
									NULL,
									error
							);

				g_dbus_node_info_unref(introspection_data);
				g_free(introspection_xml);

				return;
			}

		}

	}

}
