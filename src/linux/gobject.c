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
#include <lib3270/actions.h>
#include <lib3270/properties.h>

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>

G_DEFINE_TYPE(ipc3270, ipc3270, G_TYPE_OBJECT)

static void ipc3270_finalize(GObject *object) {

	ipc3270 * ipc = IPC3270(object);

	debug("%s",__FUNCTION__);
	if(ipc->id) {
		g_dbus_connection_unregister_object(ipc->connection,ipc->id);
	}

	G_OBJECT_CLASS(ipc3270_parent_class)->finalize(object);
}


static void ipc3270_class_init(ipc3270Class *klass) {

	debug("%s",__FUNCTION__);

	GObjectClass *object_class;
	object_class = G_OBJECT_CLASS (klass);
	object_class->finalize = ipc3270_finalize;

}

static void ipc3270_init(ipc3270 *object) {

	debug("%s",__FUNCTION__);
	object->error_domain = g_quark_from_static_string(PACKAGE_NAME);

}

GObject * ipc3270_new(GtkWidget *window, GtkWidget *terminal) {
	return g_object_new(GLIB_TYPE_IPC3270, NULL);
}

void ipc3270_set_session(GObject *object, H3270 *hSession, const char *name, GError **error) {

	char id;
	int ix;

	static const GDBusInterfaceVTable interface_vtable = {
		ipc3270_method_call,
		ipc3270_get_property,
		ipc3270_set_property
	};

	ipc3270 * ipc = IPC3270(object);
	ipc->hSession = hSession;

	ipc->connection = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, error);
	if(*error) {
		g_message("Can't get session bus: %s",(*error)->message);
		return;
	}

	g_dbus_connection_set_exit_on_close(ipc->connection,FALSE);

	for(id='a'; id < 'z' && !ipc->id && !*error; id++) {

		gchar *object_name = g_strdup_printf("br.com.bb.%s.%c",name,id);

		debug("Requesting \"%s\"",object_name);

		// https://dbus.freedesktop.org/doc/dbus-specification.html
		GError *err = NULL;

		GVariant * response =
			g_dbus_connection_call_sync (
					ipc->connection,
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
		}

		if(response) {

			guint32 reply = 0;
			g_variant_get(response, "(u)", &reply);
			g_variant_unref(response);

			if(reply == DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {

				g_message("Got %s", object_name);

				lib3270_set_session_id(ipc->hSession, id);

				// Introspection data for the service we are exporting
				GString * introspection = g_string_new(
						"<node>\n"
						"  <interface name='br.com.bb.tn3270'>"
						"    <method name='connect'>"
						"      <arg type='s' name='url' direction='in'/>"
						"    </method>"
						"    <method name='pfkey'>" \
						"      <arg type='i' name='keycode' direction='in'/>" \
						"    </method>"
						"    <method name='pakey'>" \
						"      <arg type='i' name='keycode' direction='in'/>" \
						"    </method>"
				);

				// Constrói métodos usando a tabela de controle
				const LIB3270_ACTION_ENTRY * actions = lib3270_get_action_table();
				for(ix = 0; actions[ix].name; ix++)
				{
					g_string_append_printf(
						introspection, \
						"    <method name='%s'>" \
						"    </method>", actions[ix].name
					);
				}

				// Inclui toggles
				for(ix = 0; ix < (int) LIB3270_TOGGLE_COUNT; ix++) {
					g_string_append_printf(introspection, "    <property type='i' name='%s' access='readwrite'/>", lib3270_get_toggle_name((LIB3270_TOGGLE) ix));
				}

				// Inclui propriedades.
				const LIB3270_INT_PROPERTY * proplist = lib3270_get_int_properties_list();
				for(ix = 0; proplist[ix].name; ix++) {
					g_string_append_printf(introspection, "    <property type='i' name='%s' access='%s'/>",
						proplist[ix].name,
						((proplist[ix].set == NULL) ? "read" : "readwrite")
					);
				}

				g_string_append(introspection,
					"    <property type='s' name='url' access='readwrite'/>"
					"    <property type='s' name='luname' access='read'/>"
					"  </interface>"
					"</node>"
				);

				gchar * introspection_xml = g_string_free(introspection,FALSE);
				GDBusNodeInfo * introspection_data = g_dbus_node_info_new_for_xml(introspection_xml, NULL);

				// Register object-id
				ipc->id = g_dbus_connection_register_object (
									ipc->connection,
									"/br/com/bb/tn3270",
									introspection_data->interfaces[0],
									&interface_vtable,
									ipc,
									NULL,
									error
							);

				g_dbus_node_info_unref(introspection_data);
				g_free(introspection_xml);

				break;
			}

		}

		g_free(object_name);

	}

}



