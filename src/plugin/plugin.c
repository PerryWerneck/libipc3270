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
 * Este programa está nomeado como - e possui - linhas de código.
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

 #define ENABLE_NLS
 #define GETTEXT_PACKAGE PACKAGE_NAME

 #include <libintl.h>
 #include <glib/gi18n.h>
 #include <gio/gio.h>

 #include "private.h"
 #include <v3270.h>
 #include <lib3270/ipc.h>

 int pw3270_plugin_stop(GtkWidget G_GNUC_UNUSED(*window), GtkWidget *terminal) {
	debug("%s(%p)",__FUNCTION__,g_object_get_data(G_OBJECT(terminal),"ipc-object-info"));
	g_object_set_data(G_OBJECT(terminal), "ipc-object-info", NULL);
	return 0;
 }

 int pw3270_plugin_start(GtkWidget *window, GtkWidget *terminal) {

	// Creates IPC, associate it with the terminal window
	GObject	* ipc = ipc3270_new();
	g_object_set_data_full(G_OBJECT(terminal), "ipc-object-info", ipc, g_object_unref);

	debug("Name: \"%s\"",gtk_widget_get_name(window));

	// Set session handle, this starts the IPC communication.
	GError * error = NULL;

	ipc3270_set_session(ipc,v3270_get_session(terminal));
	ipc3270_export_object(ipc,gtk_widget_get_name(window),&error);

	if(error) {

		GtkWidget *dialog =  gtk_message_dialog_new(
									GTK_WINDOW(window),
									GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT,
									GTK_MESSAGE_ERROR,
									GTK_BUTTONS_OK,
									_( "Can't start IPC Module" ));

		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),"%s",error->message);
		g_error_free(error);

		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		return 0;

	}

	char id = lib3270_get_session_id(v3270_get_session(terminal));
	if(id) {
		gchar * widget_name = g_strdup_printf("%s:%c",gtk_widget_get_name(window),id);
		v3270_set_session_name(terminal, widget_name);
		g_free(widget_name);
	}

	return 0;
 }



