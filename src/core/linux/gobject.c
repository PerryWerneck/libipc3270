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
		"		<arg type='i' name='row' direction='in' />" \
		"		<arg type='i' name='col' direction='in' />" \
		"		<arg type='s' name='text' direction='in' />" \
		"		<arg type='i' name='result' direction='out' />" \
		"	</method>" \
		"	<method name= 'getStringAt'>" \
		"		<arg type='i' name='row' direction='in' />" \
		"		<arg type='i' name='col' direction='in' />" \
		"		<arg type='i' name='len' direction='in' />" \
		"		<arg type='y' name='lf' direction='in' />" \
		"		<arg type='s' name='text' direction='out' />" \
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
		"	<property type='s' name='version' access='read'/>" \
		"	<property type='s' name='revision' access='read'/>"
	);

	// Constrói métodos usando a tabela de controle
	const LIB3270_ACTION_ENTRY * actions = lib3270_get_action_table();
	for(ix = 0; actions[ix].name; ix++)
	{
		g_string_append_printf(
			introspection, \
			"	<method name='%s'>" \
			"	</method>", actions[ix].name
		);
	}

	// Toggle properties
	for(ix = 0; ix < (int) LIB3270_TOGGLE_COUNT; ix++) {
		g_string_append_printf(introspection, "    <property type='i' name='%s' access='readwrite'/>", lib3270_get_toggle_name((LIB3270_TOGGLE) ix));
	}

	// Boolean properties
	const LIB3270_INT_PROPERTY * bol_props = lib3270_get_boolean_properties_list();
	for(ix = 0; bol_props[ix].name; ix++) {
		debug("Boolean(%s)",bol_props[ix].name);
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

	// String properties
	const LIB3270_STRING_PROPERTY * str_props = lib3270_get_string_properties_list();
	for(ix = 0; str_props[ix].name; ix++) {
		g_string_append_printf(introspection, "    <property type='s' name='%s' access='%s'/>",
			str_props[ix].name,
			((str_props[ix].set == NULL) ? "read" : "readwrite")
		);
	}

}

void ipc3270_set_session(GObject *object, H3270 *hSession) {

	ipc3270 * ipc = IPC3270(object);
	ipc->hSession = hSession;

}

const gchar * ipc3270_get_display_charset(GObject *object) {
	return lib3270_get_display_charset(IPC3270(object)->hSession);
}

H3270 * ipc3270_get_session(GObject *object) {
	return IPC3270(object)->hSession;
}

void ipc3270_set_error(GObject *object, int errcode, GError **error) {
	g_set_error(error,IPC3270(object)->error_domain,errcode,"%s",strerror(errcode));
}