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
 #include <lib3270/ipc.h>
 #include <string.h>
 #include <stdlib.h>

 /*---[ Globals ]------------------------------------------------------------------------------------*/

 const gchar * plugin_path 	= ".bin/Debug";
 const gchar * session_name	= "pw3270";
 const gchar * plugin_name	= "ipc3270." G_MODULE_SUFFIX;

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

 static void toggle_ds_trace(GtkToggleToolButton *button, GtkWidget *terminal)
 {
	v3270_set_toggle(terminal,LIB3270_TOGGLE_DS_TRACE,gtk_toggle_tool_button_get_active(button));
 }

 static void toggle_event_trace(GtkToggleToolButton *button, GtkWidget *terminal)
 {
	v3270_set_toggle(terminal,LIB3270_TOGGLE_EVENT_TRACE,gtk_toggle_tool_button_get_active(button));
 }

 static void toggle_ssl_trace(GtkToggleToolButton *button, GtkWidget *terminal)
 {
	v3270_set_toggle(terminal,LIB3270_TOGGLE_SSL_TRACE,gtk_toggle_tool_button_get_active(button));
 }

 static void toggle_screen_trace(GtkToggleToolButton *button, GtkWidget *terminal)
 {
	v3270_set_toggle(terminal,LIB3270_TOGGLE_SCREEN_TRACE,gtk_toggle_tool_button_get_active(button));
 }

 static void toggle_started_trace(GtkToggleToolButton *button, GModule *module)
 {
 	if(!module)
		return;

	GtkWidget * terminal = g_object_get_data(G_OBJECT(button),"terminal");

	const gchar * method_name = (gtk_toggle_tool_button_get_active(button) ? "pw3270_plugin_start" : "pw3270_plugin_stop");

	static void (*call)(GtkWidget *window, GtkWidget *terminal) = NULL;
	if(!g_module_symbol(module,method_name,(gpointer) &call))
	{
		g_message("Can't get method \"%s\": %s",method_name,g_module_error());
		return;
	}

	g_message("Calling %s",method_name);
	call(gtk_widget_get_toplevel(terminal), GTK_WIDGET(terminal));

 }

 static GtkToolItem * create_tool_item(GtkWidget *terminal, const gchar *label, const gchar *tooltip, GCallback callback)
 {
	GtkToolItem * item = gtk_toggle_tool_button_new();
	gtk_tool_button_set_label(GTK_TOOL_BUTTON(item),label);

	g_signal_connect(GTK_WIDGET(item), "toggled", G_CALLBACK(callback), terminal);

	if(tooltip)
		gtk_widget_set_tooltip_text(GTK_WIDGET(item),tooltip);

	return item;
 }

 static void activate(GtkApplication* app, G_GNUC_UNUSED gpointer user_data) {

	GtkWidget * window		= gtk_application_window_new(app);
	GtkWidget * terminal	= v3270_new();
	GtkWidget * notebook	= gtk_notebook_new();
	GModule   * module		= NULL;

	gtk_widget_set_name(window,session_name);

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
		GtkWidget	* box		= gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
		GtkWidget	* trace 	= v3270_trace_new(terminal);
		GtkWidget	* toolbar	= gtk_toolbar_new();
		GtkToolItem	* start		= gtk_toggle_tool_button_new();

		gtk_widget_set_sensitive(GTK_WIDGET(start),module != NULL);

		g_object_set_data(G_OBJECT(start),"terminal",terminal);

		gtk_tool_button_set_label(GTK_TOOL_BUTTON(start),"Enable");
		g_signal_connect(GTK_WIDGET(start), "toggled", G_CALLBACK(toggle_started_trace), module);
		gtk_widget_set_tooltip_text(GTK_WIDGET(start),"Start/Stop plugin module");

		gtk_toolbar_insert(GTK_TOOLBAR(toolbar), start, -1);

		gtk_toolbar_insert(GTK_TOOLBAR(toolbar),gtk_separator_tool_item_new(),-1);

		gtk_toolbar_insert(GTK_TOOLBAR(toolbar),create_tool_item(terminal, "DS Trace","Toggle DS Trace",G_CALLBACK(toggle_ds_trace)),-1);
		gtk_toolbar_insert(GTK_TOOLBAR(toolbar),create_tool_item(terminal, "Event Trace","Toggle Event Trace",G_CALLBACK(toggle_event_trace)),-1);
		gtk_toolbar_insert(GTK_TOOLBAR(toolbar),create_tool_item(terminal, "Screen Trace","Toggle Screen Trace",G_CALLBACK(toggle_screen_trace)),-1);
		gtk_toolbar_insert(GTK_TOOLBAR(toolbar),create_tool_item(terminal, "SSL Trace","Toggle SSL Trace",G_CALLBACK(toggle_ssl_trace)),-1);

		gtk_box_pack_start(GTK_BOX(box),toolbar,FALSE,FALSE,0);
		gtk_box_pack_start(GTK_BOX(box),trace,TRUE,TRUE,0);
		gtk_notebook_append_page(GTK_NOTEBOOK(notebook),box,gtk_label_new("Trace"));
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

