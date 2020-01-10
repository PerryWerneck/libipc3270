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

	g_autoptr(GError)	error = NULL;
	g_autoptr(GObject)	response = ipc3270_response_new();

	debug("%s(%s,%s)",__FUNCTION__,interface_name,object_path);

	ipc3270_method_call(G_OBJECT(user_data), method_name, parameters, response, &error);

	if(error) {

		g_dbus_method_invocation_return_gerror(invocation, error);

	} else if(ipc3270_response_has_values(response)) {

		// Convert rc to tuple.
		// It is an error if parameters is not of the right format: it must be a tuple containing the out-parameters of the D-Bus method.
		// Even if the method has a single out-parameter, it must be contained in a tuple.

		GVariant *values[] = { ipc3270_response_steal_value(response) };
		g_dbus_method_invocation_return_value(invocation, g_variant_new_tuple(values,1));

	} else {

		g_message("%s: Invalid or unexpected method call",method_name);
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

	debug("%s(%s)",__FUNCTION__,property_name);
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

	debug("%s(%s)",__FUNCTION__,property_name);
	return ipc3270_set_property(G_OBJECT(user_data), property_name, value, error);

}

static gboolean register_object(ipc3270 *ipc, const char *name, char id) {

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
	static const GDBusInterfaceVTable interface_vtable = {
		method_call,
		get_property,
		set_property
	};
	#pragma GCC diagnostic pop

	GError * error = NULL;
	gchar *ptr;

	g_autofree gchar *object_name = g_strdup_printf(PW3270_IPC_SESSION_BUS_NAME,name,id);
	g_autofree gchar *object_path = g_strdup_printf(PW3270_IPC_SESSION_BUS_PATH,name,id);

	for(ptr=object_name;*ptr;ptr++)
		*ptr = g_ascii_tolower(*ptr);

	debug("Requesting object \"%s\"",object_name);

	// https://dbus.freedesktop.org/doc/dbus-specification.html
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
				&error
		);


	if(error) {

		g_message("Can't request \"%s\": %s",object_name,error->message);
		g_error_free(error);
		return FALSE;

	}

	if(!response) {

		g_message("Empty response when requesting \"%s\"",object_name);
		return FALSE;

	}

	guint32 reply = 0;
	g_variant_get(response, "(u)", &reply);
	g_variant_unref(response);

	if(reply == DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {

		ipc->dbus.name = g_strdup(object_name);
		lib3270_set_session_id(ipc->hSession, id);
		g_message("Got %s", ipc->dbus.name);

		// Introspection data for the service we are exporting
		GString * introspection = g_string_new("<node><interface name='" PW3270_IPC_SESSION_INTERFACE_NAME "'>");
		ipc3270_add_terminal_introspection(introspection);
		g_string_append(introspection,"</interface></node>");

		gchar * introspection_xml = g_string_free(introspection,FALSE);

		// debug("\n%s\n",introspection_xml);

		GDBusNodeInfo * introspection_data = g_dbus_node_info_new_for_xml(introspection_xml, NULL);

		debug("interface=\"%s\" path=\"%s\"",PW3270_IPC_SESSION_INTERFACE_NAME,object_path);

		// Register object-id
		ipc->dbus.id = g_dbus_connection_register_object (
							ipc->dbus.connection,
							object_path,
							introspection_data->interfaces[0],
							&interface_vtable,
							ipc,
							NULL,
							&error
					);

		g_dbus_node_info_unref(introspection_data);
		g_free(introspection_xml);

		if(error) {

			g_message("Can't register object \"%s\" with interface \"%s\" and path \"%s\": %s",
						object_name,
						PW3270_IPC_SESSION_INTERFACE_NAME,
						object_path,
						error->message);
			g_error_free(error);
			return FALSE;

		}

		return TRUE;

	}

	return FALSE;
}

void ipc3270_export_object(GObject *object, const char *name, GError **error) {

	ipc3270 * ipc = IPC3270(object);

	ipc->dbus.connection = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, error);
	if(*error) {
		g_message("Can't get session bus: %s",(*error)->message);
		return;
	}

	g_dbus_connection_set_exit_on_close(ipc->dbus.connection,FALSE);

	char id = lib3270_get_session_id(ipc->hSession);

	if(id) {

		if(register_object(ipc,name,id))
			return;

	}

	for(id='a'; id < 'z' && !ipc->dbus.id && !*error; id++) {

		if(register_object(ipc,name,id))
			return;

	}

	g_message("Can't register IPC object for session \"%s\"",name);

}
