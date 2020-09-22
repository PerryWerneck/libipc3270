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
 #include <v3270/trace.h>
 #include <ipc-glib.h>
 #include <string.h>
 #include <stdlib.h>
 #include <glib.h>
 #include <glib/gstdio.h>
 #include <lib3270/toggle.h>
 #include <lib3270/ssl.h>

 /*---[ Globals ]------------------------------------------------------------------------------------*/

 #ifdef _WIN32
 const gchar * plugin_path 	= ".";
 #else
 const gchar * plugin_path 	= G_STRINGIFY(PLUGIN_PATH);
 #endif // _WIN32
 const gchar * session_name	= G_STRINGIFY(PRODUCT_NAME);
 const gchar * plugin_name	= "ipcserver." G_MODULE_SUFFIX;

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

 static void toggle_started(GtkToggleButton *button, GModule *module)
 {
 	if(!module)
		return;

	GtkWidget * terminal = g_object_get_data(G_OBJECT(button),"terminal");

	const gchar * method_name = (gtk_toggle_button_get_active(button) ? "pw3270_plugin_start" : "pw3270_plugin_stop");

	static void (*call)(GtkWidget *window, GtkWidget *terminal) = NULL;
	if(!g_module_symbol(module,method_name,(gpointer) &call))
	{
		g_message("Can't get method \"%s\": %s",method_name,g_module_error());
		return;
	}

	g_message("Calling %s",method_name);
	call(gtk_widget_get_toplevel(terminal), GTK_WIDGET(terminal));

 }

 static void session_changed(GtkWidget *widget, GtkWidget *window) {
	gtk_window_set_title(GTK_WINDOW(window),v3270_get_session_name(widget));
 }

 static void activate(GtkApplication* app, G_GNUC_UNUSED gpointer user_data) {

	GtkWidget * window		= gtk_application_window_new(app);
	GtkWidget * terminal	= v3270_new();
	GtkWidget * notebook	= gtk_notebook_new();
	GModule   * module		= NULL;

	// Hack to speed up the tests.
	lib3270_ssl_set_crl_download(v3270_get_session(terminal),0);

	gtk_widget_set_name(window,session_name);
	v3270_set_session_name(terminal,session_name);

#ifdef _WIN32
	{
		// Setup font name
		//v3270_set_font_family(terminal,"Droid Sans Mono");

		// WIN32 path settings.
		static const char * sep = "\\/.";

		TCHAR szPath[MAX_PATH];

		if(GetModuleFileName(NULL, szPath, MAX_PATH ) ) {

			for(const char *p = sep; *p; p++) {

				char *ptr = strrchr(szPath,*p);
				if(ptr) {
					*(ptr+1) = 0;
					break;
				}

			}

		}

		g_chdir(szPath);

	}
#endif // _WIN32

	// Setup tabs
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),terminal,gtk_label_new(v3270_get_session_name(terminal)));

	// Load plugin
	{
		g_autofree gchar * plugin = g_build_filename(plugin_path,plugin_name,NULL);

		g_message("Loading %s",plugin);

		module = g_module_open(plugin,G_MODULE_BIND_LOCAL);

		if(module)
		{
			g_signal_connect (terminal, "destroy", G_CALLBACK(close_module), module);
		}
		else
		{
			g_message("Can't open \"%s\": %s",plugin,g_module_error());
			gtk_main_quit();
		}

	}
	// Create trace window
	{
		GtkWidget	* trace 	= v3270_trace_new(terminal);
		GtkWidget	* start		= gtk_toggle_button_new_with_label("Enable");
		GtkWidget	* buttons	= v3270_trace_get_button_box(trace);

		gtk_widget_set_sensitive(GTK_WIDGET(start),module != NULL);

		g_object_set_data(G_OBJECT(start),"terminal",terminal);

		g_signal_connect(GTK_WIDGET(start), "toggled", G_CALLBACK(toggle_started), module);
		gtk_widget_set_tooltip_text(GTK_WIDGET(start),"Start/Stop plugin module");

		gtk_box_pack_start(GTK_BOX(buttons),start,FALSE,FALSE,0);
		gtk_box_reorder_child(GTK_BOX(buttons),start,0);

		gtk_notebook_append_page(GTK_NOTEBOOK(notebook),trace,gtk_label_new("Trace"));

		/*
		GtkWidget	* box		= gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
		GtkWidget	* trace 	= v3270_trace_new(terminal);
		GtkToolItem	* start		= gtk_toggle_tool_button_new();

		gtk_widget_set_sensitive(GTK_WIDGET(start),module != NULL);

		g_object_set_data(G_OBJECT(start),"terminal",terminal);

		gtk_tool_button_set_label(GTK_TOOL_BUTTON(start),"Enable");
		g_signal_connect(GTK_WIDGET(start), "toggled", G_CALLBACK(toggle_started_trace), module);
		gtk_widget_set_tooltip_text(GTK_WIDGET(start),"Start/Stop plugin module");

		v3270_trace_button_box_insert(trace,start);

		gtk_toolbar_insert(GTK_TOOLBAR(toolbar), start, -1);

		gtk_toolbar_insert(GTK_TOOLBAR(toolbar),gtk_separator_tool_item_new(),-1);

		gtk_box_pack_start(GTK_BOX(box),toolbar,FALSE,FALSE,0);
		gtk_box_pack_start(GTK_BOX(box),trace,TRUE,TRUE,0);
		gtk_notebook_append_page(GTK_NOTEBOOK(notebook),box,gtk_label_new("Trace"));
		*/
	}

	// Setup and show main window
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	gtk_window_set_default_size (GTK_WINDOW (window), 800, 500);
	gtk_container_add(GTK_CONTAINER(window),notebook);
	gtk_widget_show_all (window);

	// Setup title.
	const gchar *url = lib3270_get_url(v3270_get_session(terminal));
	if(url) {

		v3270_set_url(terminal,url);
		//v3270_reconnect(terminal);

		gchar * title = g_strdup_printf("%s - %s", v3270_get_session_name(terminal), url);
		gtk_window_set_title(GTK_WINDOW(window), title);
		g_free(title);

	} else {

		gtk_window_set_title(GTK_WINDOW(window), v3270_get_session_name(terminal));

	}

	g_signal_connect(terminal,"session_changed",G_CALLBACK(session_changed),window);

	lib3270_set_toggle(v3270_get_session(terminal),LIB3270_TOGGLE_EVENT_TRACE,1);

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

