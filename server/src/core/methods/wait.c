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
 * Contatos:
 *
 * perry.werneck@gmail.com	(Alexandre Perry de Souza Werneck)
 * erico.mendonca@gmail.com	(Erico Mascarenhas Mendonça)
 *
 */

#include "private.h"
#include <errno.h>
#include <string.h>
#include <lib3270/keyboard.h>

int ipc3270_method_wait_for_string(GObject *session, GVariant *request, GObject *response, GError **error) {

	H3270 *hSession = ipc3270_get_session(session);
	int rc = 0;
	guint seconds = 10;
	gchar *text = NULL;

	if(*error)
		return 0;

	switch(g_variant_n_children(request)) {
	case 2:	// Text & timeout
		{
			gchar *text = NULL;
			g_variant_get(request, "(&su)", &text, &seconds);

			if(text) {

				g_autofree gchar * converted = g_convert_with_fallback(text,-1,lib3270_get_display_charset(hSession),"UTF-8","?",NULL,NULL,error);
				rc = lib3270_wait_for_string(hSession,(const char *) converted, seconds);

			}

		}

		break;

	case 3:	// Address, text & timeout
		{
			gint addr;
			g_variant_get(request, "(i&su)", &addr, &text, &seconds);

			if(text) {

				g_autofree gchar * converted = g_convert_with_fallback(text,-1,lib3270_get_display_charset(hSession),"UTF-8","?",NULL,NULL,error);
				rc = lib3270_wait_for_string_at_address(hSession,addr,(const char *) converted, seconds);

			}

		}
		break;

	case 4:	// Row, col, text & timeout
		{
			guint row, col;
			g_variant_get(request, "(uu&su)", &row, &col, &text, &seconds);

			if(text) {

				g_autofree gchar * converted = g_convert_with_fallback(text,-1,lib3270_get_display_charset(hSession),"UTF-8","?",NULL,NULL,error);
				rc = lib3270_wait_for_string_at(hSession,row,col,converted,seconds);

			}

		}
		break;

	default:
		g_message("waitforstring was called with %u arguments.",(unsigned int) g_variant_n_children(request));
		return EINVAL;
	}

	ipc3270_response_append_int32(response, rc);

	return 0;
}

int ipc3270_method_wait(GObject *session, GVariant *request, GObject *response, GError G_GNUC_UNUSED(**error)) {
	guint seconds = 1;
	g_variant_get(request, "(u)", &seconds);
	ipc3270_response_append_int32(response,lib3270_wait(ipc3270_get_session(session),seconds));
	return 0;
}

int ipc3270_method_wait_for_ready(GObject *session, GVariant *request, GObject *response, GError G_GNUC_UNUSED(**error)) {
	guint seconds = 1;
	g_variant_get(request, "(u)", &seconds);
	ipc3270_response_append_int32(response,lib3270_wait_for_ready(ipc3270_get_session(session),seconds));
	return 0;
}

int ipc3270_method_wait_for_update(GObject *session, GVariant *request, GObject *response, GError G_GNUC_UNUSED(**error)) {
	guint seconds = 1;
	g_variant_get(request, "(u)", &seconds);
	ipc3270_response_append_int32(response,lib3270_wait_for_update(ipc3270_get_session(session),seconds));
	return 0;
}

int ipc3270_method_wait_for_keyboard_unlock(GObject *session, GVariant *request, GObject *response, GError G_GNUC_UNUSED(**error)) {
	guint seconds = 1;
	g_variant_get(request, "(u)", &seconds);
	ipc3270_response_append_int32(response,lib3270_wait_for_keyboard_unlock(ipc3270_get_session(session),seconds));
	return 0;
}

