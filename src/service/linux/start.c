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

#include "../private.h"

#define PW3270_SERVICE_DBUS_SERVICE_PATH			"/br/com/bb/tn3270/service"
#define PW3270_SERVICE_DBUS_SERVICE					"br.com.bb.tn3270.service"
#define PW3270_SERVICE_DBUS_SERVICE_INTERFACE		"br.com.bb.tn3270.service"

static const gchar introspection_xml[] =
  "<node>"
  "  <interface name='" PW3270_SERVICE_DBUS_SERVICE_INTERFACE "'>"
  "    <property type='s' name='version' access='read'/>"
  "  </interface>"
  "</node>";

static GDBusNodeInfo *introspection_data = NULL;
static guint owner_id = 0;

static void on_bus_acquired (GDBusConnection *connection, const gchar *name, gpointer user_data) {

	static const GDBusInterfaceVTable interface_vtable = {
		service_method_call,
		service_get_property,
		service_set_property
	};

	guint registration_id;


	registration_id = g_dbus_connection_register_object (connection,
													   PW3270_SERVICE_DBUS_SERVICE_PATH,
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

	introspection_data = g_dbus_node_info_new_for_xml(introspection_xml, NULL);

	owner_id = g_bus_own_name (G_BUS_TYPE_SESSION,
                             PW3270_SERVICE_DBUS_SERVICE,
                             G_BUS_NAME_OWNER_FLAGS_NONE,
                             on_bus_acquired,
                             on_name_acquired,
                             on_name_lost,
                             NULL,
                             NULL);


}

