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
 * Este programa está nomeado como - e possui - linhas de código.
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

#include <internals.h>
#include "gobject.h"
#include <lib3270.h>
#include <lib3270/actions.h>
#include <lib3270/properties.h>
#include <lib3270/toggle.h>
#include <v3270.h>

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>

/*--[ Widget definition ]----------------------------------------------------------------------------*/

G_DEFINE_TYPE(ipc3270, ipc3270, G_TYPE_OBJECT)

/*--[ Implement ]------------------------------------------------------------------------------------*/

static void ipc3270_finalize(GObject *object) {

	ipc3270 * ipc = IPC3270(object);

	ipc3270_release_object(ipc);

	g_autofree gchar * widget_name = g_strdup(v3270_get_session_name(ipc->terminal));
	gchar 			 * ptr = strrchr(widget_name,':');
	if(ptr)
		*ptr = 0;

	lib3270_set_session_id(ipc->hSession, 0);
	v3270_set_session_name(ipc->terminal,widget_name);

	g_free(ipc->charset);

	G_OBJECT_CLASS(ipc3270_parent_class)->finalize(object);

}


static void ipc3270_class_init(ipc3270Class *klass) {

	debug("%s",__FUNCTION__);

	GObjectClass *object_class;
	object_class = G_OBJECT_CLASS (klass);
	object_class->finalize = ipc3270_finalize;

	debug("Localedir: \"%s\"",G_STRINGIFY(LOCALEDIR));
	bindtextdomain(GETTEXT_PACKAGE, G_STRINGIFY(LOCALEDIR));

}

static void ipc3270_init(ipc3270 *object) {

	debug("%s",__FUNCTION__);
	object->error_domain = g_quark_from_static_string(PACKAGE_NAME);

	// Get charset
	const gchar * scharset = NULL;
	g_get_charset(&scharset);

	object->charset = g_strdup(scharset);

}

GObject * ipc3270_new() {
	return g_object_new(GLIB_TYPE_IPC3270, NULL);
}

void ipc3270_add_terminal_introspection(GString *introspection) {

	size_t ix;

	g_string_append(introspection,
		"	<method name='connect'>"
		"		<arg type='s' name='url' direction='in'/>"
		"		<arg type='i' name='result' direction='out' />" \
		"	</method>"
		"	<method name='disconnect'>"
		"		<arg type='i' name='result' direction='out' />" \
		"	</method>"
		"	<method name='action'>"
		"		<arg type='s' name='name' direction='in' />" \
		"		<arg type='i' name='result' direction='out' />" \
		"	</method>"
		"	<method name='activatable'>"
		"		<arg type='s' name='name' direction='in' />" \
		"		<arg type='b' name='result' direction='out' />" \
		"	</method>"
		"	<method name='pfkey'>" \
		"		<arg type='i' name='keycode' direction='in'/>" \
		"		<arg type='i' name='result' direction='out' />" \
		"	</method>"
		"	<method name='pakey'>" \
		"		<arg type='i' name='keycode' direction='in'/>" \
		"		<arg type='i' name='result' direction='out' />" \
		"	</method>"
		"	<method name='getString'>" \
		"		<arg type='s' name='text' direction='out' />" \
		"	</method>" \
		"	<method name='setString'>" \
		"		<arg type='s' name='text' direction='in' />" \
		"		<arg type='i' name='result' direction='out' />" \
		"	</method>" \
		"	<method name='setStringAt'>" \
		"		<arg type='u' name='row' direction='in' />" \
		"		<arg type='u' name='col' direction='in' />" \
		"		<arg type='s' name='text' direction='in' />" \
		"		<arg type='i' name='result' direction='out' />" \
		"	</method>" \
		"	<method name= 'getStringAt'>" \
		"		<arg type='u' name='row' direction='in' />" \
		"		<arg type='u' name='col' direction='in' />" \
		"		<arg type='i' name='len' direction='in' />" \
		"		<arg type='y' name='lf' direction='in' />" \
		"		<arg type='s' name='text' direction='out' />" \
		"	</method>" \
		"	<method name='setField'>" \
		"		<arg type='s' name='text' direction='in' />" \
		"		<arg type='i' name='result' direction='out' />" \
		"	</method>" \
		"	<method name='setStringAtAddress'>" \
		"		<arg type='i' name='addr' direction='in' />" \
		"		<arg type='s' name='text' direction='in' />" \
		"		<arg type='i' name='result' direction='out' />" \
		"	</method>" \
		"	<method name= 'getStringAtAddress'>" \
		"		<arg type='i' name='addr' direction='in' />" \
		"		<arg type='i' name='len' direction='in' />" \
		"		<arg type='y' name='lf' direction='in' />" \
		"		<arg type='s' name='text' direction='out' />" \
		"	</method>" \
		"	<method name= 'wait'>" \
		"		<arg type='u' name='seconds' direction='in' />" \
		"		<arg type='i' name='result' direction='out' />" \
		"	</method>" \
		"	<method name= 'waitForReady'>" \
		"		<arg type='u' name='seconds' direction='in' />" \
		"		<arg type='i' name='result' direction='out' />" \
		"	</method>" \
		"	<method name= 'waitForKeyboardUnlock'>" \
		"		<arg type='u' name='seconds' direction='in' />" \
		"		<arg type='i' name='result' direction='out' />" \
		"	</method>" \
		"	<method name= 'waitForUpdate'>" \
		"		<arg type='u' name='seconds' direction='in' />" \
		"		<arg type='i' name='result' direction='out' />" \
		"	</method>" \
		"	<method name= 'setCursorPosition'>" \
		"		<arg type='u' name='row' direction='in' />" \
		"		<arg type='u' name='col' direction='in' />" \
		"		<arg type='i' name='old' direction='out' />" \
		"	</method>" \
		"	<method name= 'setCursorAddress'>" \
		"		<arg type='i' name='addr' direction='in' />" \
		"		<arg type='i' name='old' direction='out' />" \
		"	</method>" \
		"	<method name= 'getFieldAttribute'>" \
		"		<arg type='u' name='attribute' direction='out' />" \
		"	</method>" \
		"	<method name= 'getFieldAttributeAt'>" \
		"		<arg type='u' name='row' direction='in' />" \
		"		<arg type='u' name='col' direction='in' />" \
		"		<arg type='u' name='attribute' direction='out' />" \
		"	</method>" \
		"	<method name= 'getFieldAttributeAtAddress'>" \
		"		<arg type='i' name='addr' direction='in' />" \
		"		<arg type='u' name='attribute' direction='out' />" \
		"	</method>" \

		"	<method name= 'waitForString'>" \
		"		<arg type='s' name='text' direction='in' />" \
		"		<arg type='u' name='seconds' direction='in' />" \
		"		<arg type='i' name='result' direction='out' />" \
		"	</method>" \
		"	<method name= 'waitForStringAt'>" \
		"		<arg type='u' name='row' direction='in' />" \
		"		<arg type='u' name='col' direction='in' />" \
		"		<arg type='s' name='text' direction='in' />" \
		"		<arg type='u' name='seconds' direction='in' />" \
		"		<arg type='i' name='result' direction='out' />" \
		"	</method>" \
		"	<method name= 'waitForStringAtAddress'>" \
		"		<arg type='i' name='addr' direction='in' />" \
		"		<arg type='s' name='text' direction='in' />" \
		"		<arg type='u' name='seconds' direction='in' />" \
		"		<arg type='i' name='result' direction='out' />" \
		"	</method>" \

		"	<property type='s' name='version' access='read'/>" \
		"	<property type='s' name='revision' access='read'/>"
	);

	// Constrói métodos usando a tabela de controle
	const LIB3270_ACTION * actions = lib3270_get_actions();
	for(ix = 0; actions[ix].name; ix++)
	{
		g_string_append_printf(
			introspection, \
			"	<method name='%s'>" \
			"		<arg type='i' name='result' direction='out' />" \
			"	</method>", actions[ix].name
		);
	}

	// Toggle properties
	for(ix = 0; ix < (int) LIB3270_TOGGLE_COUNT; ix++) {
		g_string_append_printf(introspection, "    <property type='b' name='%s' access='readwrite'/>", lib3270_get_toggle_name((LIB3270_TOGGLE_ID) ix));
	}

	// Boolean properties
	const LIB3270_INT_PROPERTY * bol_props = lib3270_get_boolean_properties_list();
	for(ix = 0; bol_props[ix].name; ix++) {
//		debug("Boolean(%s)",bol_props[ix].name);
		g_string_append_printf(introspection, "    <property type='b' name='%s' access='%s'/>",
			bol_props[ix].name,
			((bol_props[ix].set == NULL) ? "read" : "readwrite")
		);
	}

	// Integer properties
	const LIB3270_INT_PROPERTY * int_props = lib3270_get_int_properties_list();
	for(ix = 0; int_props[ix].name; ix++) {
		g_string_append_printf(introspection, "    <property type='i' name='%s' access='%s'/>",
			int_props[ix].name,
			((int_props[ix].set == NULL) ? "read" : "readwrite")
		);
	}

	// Unsigned integer properties
	const LIB3270_UINT_PROPERTY * uint_props = lib3270_get_unsigned_properties_list();
	for(ix = 0; uint_props[ix].name; ix++) {
		g_string_append_printf(introspection, "    <property type='u' name='%s' access='%s'/>",
			uint_props[ix].name,
			((uint_props[ix].set == NULL) ? "read" : "readwrite")
		);
	}

	// String properties
	const LIB3270_STRING_PROPERTY * str_props = lib3270_get_string_properties_list();
	for(ix = 0; str_props[ix].name; ix++) {
		g_string_append_printf(introspection, "    <property type='s' name='%s' access='%s'/>",
			str_props[ix].name,
			((str_props[ix].set == NULL) ? "read" : "readwrite")
		);
	}

}

void ipc3270_set_terminal_widget(GObject *object, GtkWidget *terminal) {

	ipc3270 * ipc = IPC3270(object);

	ipc->terminal = terminal;
	ipc->hSession = v3270_get_session(terminal);

}

const gchar * ipc3270_get_display_charset(GObject *object) {
	return lib3270_get_display_charset(IPC3270(object)->hSession);
}

H3270 * ipc3270_get_session(GObject *object) {
	return IPC3270(object)->hSession;
}

void ipc3270_set_error(GObject *object, int errcode, GError **error) {
	if(error && !*error)
		g_set_error(error,IPC3270(object)->error_domain,errcode,"%s",strerror(errcode));
}

GQuark ipc3270_get_error_domain(GObject *object) {
	return IPC3270(object)->error_domain;
}

gchar * ipc3270_convert_to_3270(GObject *object, const gchar *string, GError **error) {
	return g_convert_with_fallback(string,-1,lib3270_get_display_charset(IPC3270(object)->hSession),IPC3270(object)->charset,"?",NULL,NULL,error);
}

gchar * ipc3270_convert_from_3270(GObject *object, const gchar *string, GError **error) {
	return g_convert_with_fallback(string,-1,IPC3270(object)->charset,lib3270_get_display_charset(IPC3270(object)->hSession),"?",NULL,NULL,error);
}
