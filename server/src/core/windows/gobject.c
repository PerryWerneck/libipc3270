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
 * Este programa está nomeado como gobject.c e possui - linhas de código.
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
#include <lib3270/log.h>
#include <ipc-glib.h>
#include <v3270.h>

/*--[ Widget definition ]----------------------------------------------------------------------------*/

G_DEFINE_TYPE(ipc3270, ipc3270, G_TYPE_OBJECT)

/*--[ Implement ]------------------------------------------------------------------------------------*/

static void ipc3270_finalize(GObject *object) {

	ipc3270 * ipc = IPC3270(object);

	debug("%s(%p)",__FUNCTION__,object);

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

	G_OBJECT_CLASS (klass)->finalize = ipc3270_finalize;

	{
		g_autofree gchar * appdir = g_win32_get_package_installation_directory_of_module(NULL);
		g_autofree gchar * locdir = g_build_filename(appdir,"locale",NULL);
		bindtextdomain(GETTEXT_PACKAGE,locdir);
	}

}

static void ipc3270_init(ipc3270 *object) {

	debug("%s(%p)",__FUNCTION__,object);
	object->error_domain = g_quark_from_static_string(PACKAGE_NAME);

	// Get charset
	const gchar * scharset = NULL;
	g_get_charset(&scharset);

	object->charset = g_strdup(scharset);

}

GObject * ipc3270_new() {
	return g_object_new(GLIB_TYPE_IPC3270, NULL);
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


