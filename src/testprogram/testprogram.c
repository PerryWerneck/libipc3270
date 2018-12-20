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
  * @brief Test program for pw3270 IPC plugin.
  *
  */

 #include "../private.h"
 #include <stdlib.h>

 /*---[ Implement ]----------------------------------------------------------------------------------*/

static void activate(GtkApplication* app, gpointer user_data) {

	GtkWidget	* window	= gtk_application_window_new(app);
	GtkWidget	* terminal	= v3270_new();

	gtk_widget_set_name(window,"pw3270");

	// Setup and show window
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	gtk_window_set_default_size (GTK_WINDOW (window), 800, 500);
	gtk_container_add(GTK_CONTAINER(window),terminal);
	gtk_widget_show_all (window);

	pw3270_plugin_start(window, terminal);

	const gchar *url = getenv("LIB3270_DEFAULT_HOST");
	if(url) {

		v3270_set_url(terminal,url);
		v3270_connect(terminal);
		gchar * title = g_strdup_printf("%s - %s", v3270_get_session_name(terminal), url);
		gtk_window_set_title(GTK_WINDOW(window), title);
		g_free(title);

	} else {

		gtk_window_set_title(GTK_WINDOW(window), v3270_get_session_name(terminal));

	}

}

int main (int argc, char **argv) {

  GtkApplication *app;
  int status;

  app = gtk_application_new ("br.com.bb.pw3270",G_APPLICATION_FLAGS_NONE);

  g_signal_connect (app, "activate", G_CALLBACK(activate), NULL);

  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  g_message("rc=%d",status);
  return status;
}

