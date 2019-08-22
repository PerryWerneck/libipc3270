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
 * Este programa está nomeado como start.c e possui - linhas de código.
 *
 * Referências:
 *
 * https://github.com/joprietoe/gdbus/blob/master/gdbus-example-server.c
 * https://github.com/bratsche/glib/blob/master/gio/tests/gdbus-example-export.c
 *
 * Contatos:
 *
 * perry.werneck@gmail.com	(Alexandre Perry de Souza Werneck)
 *
 */

#include <config.h>
#include <ipc-glib.h>
#include "../private.h"

static GDBusNodeInfo *introspection_data = NULL;
static guint owner_id = 0;
static gchar * introspection_xml = NULL;

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

	debug("%s(%s)",__FUNCTION__,object_path);

	GVariant * rc = service_method_call(method_name, parameters, &error);

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

	debug("%s(%s)",__FUNCTION__,object_path);
	return service_get_property(property_name, error);

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

	debug("%s(%s)",__FUNCTION__,object_path);
	return service_set_property(property_name, value, error);

}


static void on_bus_acquired (GDBusConnection *connection, const gchar *name, gpointer user_data) {

	static const GDBusInterfaceVTable interface_vtable = {
		method_call,
		get_property,
		set_property
	};

	guint registration_id;

	g_message("Registering object %s",PW3270_IPC_SERVICE_OBJECT_PATH);

	registration_id = g_dbus_connection_register_object (connection,
													   PW3270_IPC_SERVICE_OBJECT_PATH,
													   introspection_data->interfaces[0],
													   &interface_vtable,
													   NULL,  /* user_data */
													   NULL,  /* user_data_free_func */
													   NULL); /* GError** */
	g_assert (registration_id > 0);

}

static void on_name_acquired(GDBusConnection *connection, const gchar *name, gpointer user_data) {

	g_message("Acquired %s",name);

}

static void on_name_lost (GDBusConnection *connection, const gchar *name, gpointer user_data) {
	g_message("Lost %s",name);
    g_main_loop_quit(NULL);
}

void service_start(void) {

	GString * introspection = g_string_new("<node>\n");

	g_string_append(introspection,
		"	<interface name='" PW3270_IPC_SERVICE_INTERFACE_NAME "'>"
		"		<method name='createSession'>"
		"			<arg type='s' name='id' direction='out' />"
		"		</method>"
		"		<method name='destroySession'>"
		"			<arg type='s' name='id' direction='in' />"
		"			<arg type='i' name='rc' direction='out' />"
		"		</method>"
		"		<property type='s' name='version' access='read'/>"
		"		<property type='s' name='release' access='read'/>"
	);

	ipc3270_add_terminal_introspection(introspection);

	g_string_append(introspection,"</interface></node>\n");

	introspection_xml = g_string_free(introspection,FALSE);

	debug("\n\n%s\n\n",introspection_xml);

	introspection_data = g_dbus_node_info_new_for_xml(introspection_xml, NULL);

	owner_id = g_bus_own_name (G_BUS_TYPE_SESSION,
                             PW3270_IPC_SERVICE_BUS_NAME,
                             G_BUS_NAME_OWNER_FLAGS_NONE,
                             on_bus_acquired,
                             on_name_acquired,
                             on_name_lost,
                             NULL,
                             NULL);


}

