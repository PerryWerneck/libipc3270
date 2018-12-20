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
#include <v3270.h>

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>

/* Introspection data for the service we are exporting */
static const gchar introspection_xml[] =
	"<node>"
	"  <interface name='br.com.bb.tn3270'>"
	"    <method name='about'>"
	"      <arg type='s' name='revision' direction='out'/>"
	"    </method>"
	"  </interface>"
	"</node>";

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

}

void
ipc3270_method_call (GDBusConnection       *connection,
                    const gchar           *sender,
                    const gchar           *object_path,
                    const gchar           *interface_name,
                    const gchar           *method_name,
                    GVariant              *parameters,
                    GDBusMethodInvocation *invocation,
                    gpointer               user_data)
{


	g_dbus_method_invocation_return_error (
		invocation,
		G_DBUS_ERROR,
		G_DBUS_ERROR_UNKNOWN_METHOD,
		"Invalid or unexpected method call");

}

GVariant *
ipc3270_get_property (GDBusConnection  *connection,
                     const gchar      *sender,
                     const gchar      *object_path,
                     const gchar      *interface_name,
                     const gchar      *property_name,
                     GError          **error,
                     gpointer          user_data)
{
	GVariant *ret = NULL;

	g_set_error (error,
		G_IO_ERROR,
		G_IO_ERROR_NOT_FOUND,
		"There's no %s propriety", property_name
	);

	return ret;
}

gboolean
ipc3270_set_property (GDBusConnection  *connection,
                     const gchar      *sender,
                     const gchar      *object_path,
                     const gchar      *interface_name,
                     const gchar      *property_name,
                     GVariant         *value,
                     GError          **error,
                     gpointer          user_data)
{


	g_set_error (error,
		G_IO_ERROR,
		G_IO_ERROR_NOT_FOUND,
		"There's no %s propriety", property_name
	);

	return *error == NULL;
}

GObject * ipc3270_new(GtkWidget *window, GtkWidget *terminal) {

	static const GDBusInterfaceVTable interface_vtable = {
		ipc3270_method_call,
		ipc3270_get_property,
		ipc3270_set_property
	};

	ipc3270 * object = IPC3270(g_object_new(GLIB_TYPE_IPC3270, NULL));

	GError 				* error = NULL;
	int					  id;

	object->connection = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &error);

	if(error) {
		GtkWidget *dialog =  gtk_message_dialog_new(
									GTK_WINDOW(window),
									GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT,
									GTK_MESSAGE_ERROR,
									GTK_BUTTONS_OK,
									_( "Can't get D-Bus connection" ));

		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),"%s",error->message);
		g_error_free(error);

		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);

		return G_OBJECT(object);

	}

	g_dbus_connection_set_exit_on_close(object->connection,FALSE);

	for(id='a'; id < 'z' && !object->id && !error; id++) {

		gchar *name = g_strdup_printf("br.com.bb.%s.%c",gtk_widget_get_name(window),id);

		debug("Requesting \"%s\"",name);

		// https://dbus.freedesktop.org/doc/dbus-specification.html
		GVariant * response =
			g_dbus_connection_call_sync (
					object->connection,
					DBUS_SERVICE_DBUS,
					DBUS_PATH_DBUS,
					DBUS_INTERFACE_DBUS,
					"RequestName",
					g_variant_new ("(su)", name, DBUS_NAME_FLAG_DO_NOT_QUEUE),
					NULL,
					G_DBUS_CALL_FLAGS_NONE,
					-1,
					NULL,
					&error
			);

		if(error) {
			g_message("Can't request \"%s\": %s",name,error->message);
			g_error_free(error);
			error = NULL;
		}

		if(response) {

			guint32 reply = 0;
			g_variant_get(response, "(u)", &reply);
			g_variant_unref(response);

			if(reply == DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {

				gchar * widget_name = g_strdup_printf("%s:%c",gtk_widget_get_name(window),id);
				v3270_set_session_name(terminal, widget_name);
				g_free(widget_name);

				g_message("Got %s - %s", name, v3270_get_session_name(terminal));

				GDBusNodeInfo * introspection_data = g_dbus_node_info_new_for_xml(introspection_xml, NULL);

				// Register object-id
				object->id = g_dbus_connection_register_object (
									object->connection,
									"/br/com/bb/tn3270",
									introspection_data->interfaces[0],
									&interface_vtable,
									object,
									NULL,
									&error
							);

				g_dbus_node_info_unref(introspection_data);

				break;
			}

		}

		g_free(name);

	}

	if(error) {

		GtkWidget *dialog =  gtk_message_dialog_new(
									GTK_WINDOW(window),
									GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT,
									GTK_MESSAGE_ERROR,
									GTK_BUTTONS_OK,
									_( "Can't register D-Bus Object" ));

		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),"%s",error->message);
		g_error_free(error);

		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);

		return G_OBJECT(object);

	}

	return G_OBJECT(object);

}
