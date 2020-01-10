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
  * @brief IPC Plugin startup/stop.
  *
  */

 #include "private.h"
 #include <v3270.h>
 #include <ipc-glib.h>

 int pw3270_plugin_page_added(GtkWidget *terminal) {

 	if(!GTK_IS_V3270(terminal))
		return EINVAL;


 	// Creates IPC, associate it with the terminal window.
 	// Build session name.
 	g_autofree gchar * session_name = g_strdup(v3270_get_session_name(terminal));
 	{
 		gchar *ptr = strchr(session_name,':');
 		if(ptr)
			*ptr = 0;

		for(ptr=session_name;*ptr;ptr++) {

			if(!g_ascii_isalnum(*ptr)) {
				*ptr = '_';
			} else {
				*ptr = g_ascii_tolower(*ptr);
			}

		}

 	}

 	debug("Session name for widget %p is \"%s\"",terminal,session_name);

	// Create IPC object
	GObject	* ipc = ipc3270_new();
	g_object_set_data_full(G_OBJECT(terminal), "ipc-object-info", ipc, g_object_unref);

	// Set session handle, this starts the IPC communication.
	GError * error = NULL;

	ipc3270_set_terminal_widget(ipc,terminal);
	ipc3270_export_object(ipc,session_name,&error);

	if(error) {

		GtkWidget *dialog =  gtk_message_dialog_new(
									GTK_WINDOW(gtk_widget_get_toplevel(terminal)),
									GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT,
									GTK_MESSAGE_ERROR,
									GTK_BUTTONS_OK,
									_( "Can't start IPC Module" ));

		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),"%s",error->message);
		g_error_free(error);

        g_signal_connect(dialog,"close",G_CALLBACK(gtk_widget_destroy),NULL);
        g_signal_connect(dialog,"response",G_CALLBACK(gtk_widget_destroy),NULL);

        gtk_widget_show_all(dialog);

		return -1;

	}

	char id = lib3270_get_session_id(v3270_get_session(terminal));
	if(id) {
		g_autofree gchar * widget_name = g_strdup_printf("%s:%c",session_name,id);
		v3270_set_session_name(terminal, widget_name);
	}

 	return 0;

 }

 int pw3270_plugin_page_removed(GtkWidget *terminal) {

 	if(!GTK_IS_V3270(terminal))
		return EINVAL;

	debug("%s(%p)",__FUNCTION__,g_object_get_data(G_OBJECT(terminal),"ipc-object-info"));
	g_object_set_data(G_OBJECT(terminal), "ipc-object-info", NULL);
	return 0;

 }

 int pw3270_plugin_start(GtkWidget G_GNUC_UNUSED(*window), GtkWidget *terminal) {
	return pw3270_plugin_page_added(terminal);
 }

 int pw3270_plugin_stop(GtkWidget G_GNUC_UNUSED(*window), GtkWidget *terminal) {
 	return pw3270_plugin_page_removed(terminal);
 }



