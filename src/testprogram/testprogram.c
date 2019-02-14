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

 #include <config.h>
 #include <v3270.h>
 #include <lib3270/ipc.h>
 #include <string.h>
 #include <stdlib.h>

 /*---[ Globals ]------------------------------------------------------------------------------------*/

 const gchar * plugin_path = ".bin/Debug";
 const gchar * plugin_name = "ipc3270." G_MODULE_SUFFIX;

 /*---[ Implement ]----------------------------------------------------------------------------------*/

 static void close_module(GtkWidget *widget, GModule *module)
 {
 	g_message("Closing module %p",module);

	static void (*stop)(GtkWidget *window, GtkWidget *terminal) = NULL;
	if(!g_module_symbol(module,"pw3270_plugin_stop",(gpointer) &stop))
	{
		g_message("Can't get stop method from plugin: %s",g_module_error());
	}
	else
	{
		stop(gtk_widget_get_toplevel(widget),widget);
	}

	g_module_close(module);
	g_message("Module %p was closed",module);
 }

 static void activate(GtkApplication* app, G_GNUC_UNUSED gpointer user_data) {

	GtkWidget	* window	= gtk_application_window_new(app);
	GtkWidget	* terminal	= v3270_new();

	gtk_widget_set_name(window,"pw3270");

	// Setup and show window
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	gtk_window_set_default_size (GTK_WINDOW (window), 800, 500);
	gtk_container_add(GTK_CONTAINER(window),terminal);
	gtk_widget_show_all (window);

	// Load plugin
	{
		g_autofree gchar * plugin = g_build_filename(plugin_path,plugin_name,NULL);

		g_message("Loading %s",plugin);

		GModule * module = g_module_open(plugin,G_MODULE_BIND_LOCAL);

		if(module)
		{
			g_signal_connect (terminal, "destroy", G_CALLBACK(close_module), module);
		}
		else
		{
			g_message("Can't open \"%s\": %s",plugin,g_module_error());
			gtk_main_quit();
		}

		static void (*start)(GtkWidget *window, GtkWidget *terminal) = NULL;
		if(!g_module_symbol(module,"pw3270_plugin_start",(gpointer) &start))
		{
			g_message("Can't get start method from \"%s\": %s",plugin,g_module_error());
			gtk_main_quit();
		}

		g_message("Starting plugin %p",module);
		start(window,terminal);

	}

	// Setup title.
	const gchar *url = lib3270_get_url(v3270_get_session(terminal));
	if(url) {

		v3270_set_url(terminal,url);
		v3270_reconnect(terminal);

		gchar * title = g_strdup_printf("%s - %s", v3270_get_session_name(terminal), url);
		gtk_window_set_title(GTK_WINDOW(window), title);
		g_free(title);

	} else {

		gtk_window_set_title(GTK_WINDOW(window), v3270_get_session_name(terminal));

	}

}

int main (int argc, char **argv) {

	/*
	GVariantBuilder builder;

	g_variant_builder_init(&builder,G_VARIANT_TYPE("(isi)"));

	g_variant_builder_add(&builder, "i", 10);
	g_variant_builder_add(&builder, "s", "teste");
	g_variant_builder_add(&builder, "i", 20);

	GVariant *value = g_variant_builder_end(&builder);

	size_t szPacket = 0;
	g_autofree unsigned char * buffer = ipc3270_pack("teste", value, &szPacket);
	g_variant_unref(value);

	debug("Package \"%s\" was created with %u bytes", buffer, (unsigned int) szPacket);

	value = ipc3270_unpack(buffer);

	g_variant_unref(value);
	*/

	GtkApplication *app;
	int status;

	app = gtk_application_new ("br.com.bb.pw3270",G_APPLICATION_FLAGS_NONE);

	g_signal_connect (app, "activate", G_CALLBACK(activate), NULL);

	status = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref (app);

	g_message("rc=%d",status);
	return status;

}

