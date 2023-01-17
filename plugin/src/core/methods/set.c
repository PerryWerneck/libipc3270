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

int ipc3270_method_set_field_contents(GObject *session, GVariant *request, GObject *response, GError **error)
{
	H3270 *hSession = ipc3270_get_session(session);

	if(*error)
		return 0;

	if(g_variant_n_children(request) != 1)
		return EINVAL;

	gchar *text = NULL;
	g_variant_get(request, "(&s)", &text);

	if(!text)
		return EINVAL;

	g_autofree gchar * converted = g_convert_with_fallback(text,-1,lib3270_get_display_charset(hSession),"UTF-8","?",NULL,NULL,error);

	int rc = lib3270_set_field(hSession,converted,-1);

	if(rc < 0) {
		debug("lib3270_set_field has failed: %s", strerror(-rc));
		return -rc;
	}

	ipc3270_response_append_int32(response, rc);

	return 0;

}

int ipc3270_method_set_string(GObject *session, GVariant *request, GObject *response, GError **error) {

	H3270 *hSession = ipc3270_get_session(session);

	if(*error)
		return 0;

	switch(g_variant_n_children(request)) {
	case 1:	// Just text
		{
			gchar *text = NULL;
			g_variant_get(request, "(&s)", &text);

			if(text) {

				g_autofree gchar * converted = g_convert_with_fallback(text,-1,lib3270_get_display_charset(hSession),"UTF-8","?",NULL,NULL,error);

				if(lib3270_input_string(hSession,(const unsigned char *) converted, -1)) {
					debug("lib3270_input_string has failed: %s", strerror(errno));
					return errno;
				}

			}

		}

		break;

	case 2:	// Address and text
		{
			gint addr;
			gchar *text = NULL;
			g_variant_get(request, "(i&s)", &addr, &text);

			if(text) {

				g_autofree gchar * converted = g_convert_with_fallback(text,-1,lib3270_get_display_charset(hSession),"UTF-8","?",NULL,NULL,error);

				if(lib3270_set_string_at_address(hSession,addr,(unsigned char *) converted, -1) < 0)
					return errno;

			}

		}
		break;

	case 3:	// Row, col & text
		{
			guint row, col;
			gchar *text = NULL;
			g_variant_get(request, "(uu&s)", &row, &col, &text);

			if(text) {

				g_autofree gchar * converted = g_convert_with_fallback(text,-1,lib3270_get_display_charset(hSession),"UTF-8","?",NULL,NULL,error);

				if(lib3270_set_string_at(hSession, row, col, (unsigned char *) converted, -1) < 0)
					return errno;

			}

		}
		break;

	default:
		g_message("setstring was called with %u arguments.",(unsigned int) g_variant_n_children(request));
		return EINVAL;
	}

	ipc3270_response_append_int32(response, 0);

	return 0;
}

