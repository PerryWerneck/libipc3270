/*
 * "Software pw3270, desenvolvido com base nos códigos fontes do WC3270  e X3270
 * (Paul Mattes Paul.Mattes@usa.net), de emulação de terminal 3270 para acesso a
 * aplicativos mainframe. Registro no INPI sob o nome G3270. Registro no INPI sob
 * o nome G3270.
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
 * Este programa está nomeado como testprogram.c e possui - linhas de código.
 *
 * Contatos:
 *
 * perry.werneck@gmail.com	(Alexandre Perry de Souza Werneck)
 * erico.mendonca@gmail.com	(Erico Mascarenhas Mendonça)
 *
 */


 /**
  * @brief Plugin startup/stop for linux.
  *
  */

 #include "../private.h"

 #ifndef _WIN32

 #include <dbus/dbus-glib.h>
 #include <dbus/dbus-glib-bindings.h>

 static void pw3270_dbus_cleanup(struct DBusSession	* dBus) {

 	if(dBus->proxy) {
		g_object_unref(dBus);
 	}

 }

 int pw3270_plugin_start(GtkWidget *window, GtkWidget *terminal) {

	struct DBusSession	* dBus = g_new0(struct DBusSession,1);
	GError 				* error = NULL;
	int					  id;

	g_object_set_data_full(G_OBJECT(terminal), "dbus-session-info", dBus, (GDestroyNotify) pw3270_dbus_cleanup);

	dBus->connection = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &error);

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

		return -1;
	}

	g_dbus_connection_set_exit_on_close(dBus->connection,FALSE);

	/*
	dBus->proxy = g_dbus_proxy_new_sync(
			dBus->connection,
			G_DBUS_PROXY_FLAGS_NONE,
			NULL,									// GDBusInterfaceInfo
			"br.com.bb." PACKAGE_NAME,				// name
			"/br/com/bb/" PACKAGE_NAME "/terminal",	// object path
			"br.com.bb." PACKAGE_NAME ".terminal",	// interface
			NULL,									// GCancellable
			&error );

	if(error) {

		GtkWidget *dialog =  gtk_message_dialog_new(
									GTK_WINDOW(window),
									GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT,
									GTK_MESSAGE_ERROR,
									GTK_BUTTONS_OK,
									_( "Can't get D-Bus proxy object" ));

		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),"%s",error->message);
		g_error_free(error);

		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);

		return -1;
	}
	*/

	for(id='a'; id < 'z' && !error && !dBus->proxy; id++) {

		gchar *name = g_strdup_printf("br.com.bb.%s.%c",gtk_widget_get_name(window),id);

		debug("Requesting \"%s\"",name);

		// https://dbus.freedesktop.org/doc/dbus-specification.html
		GVariant * response =
			g_dbus_connection_call_sync (
					dBus->connection,
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

				dBus->proxy = g_dbus_proxy_new_sync(
						dBus->connection,
						G_DBUS_PROXY_FLAGS_NONE,
						NULL,									// GDBusInterfaceInfo
						name,									// name
						"/br/com/bb/" PACKAGE_NAME "/terminal",	// object path
						"br.com.bb." PACKAGE_NAME ".terminal",	// interface
						NULL,									// GCancellable
						&error );

				gchar * widget_name = g_strdup_printf("%s:%c",gtk_widget_get_name(window),id);
				v3270_set_session_name(terminal, widget_name);
				g_free(widget_name);

				g_message("Got %s - %s", name, v3270_get_session_name(terminal));

			}

		}

		g_free(name);

	}


	return 0;

 }

 #endif // !_WIN32


