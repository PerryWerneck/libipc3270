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

int ipc3270_method_get_string(GObject *session, GVariant *request, GObject *response, GError **error) {

	H3270 *hSession = ipc3270_get_session(session);

	lib3270_autoptr(char) text = NULL;

	switch(g_variant_n_children(request)) {
	case 0: // No arguments
		{
			text = lib3270_get_string_at_address(hSession, 0, -1, '\n');
		}
		break;

	case 3:	// address, length, line-delimiter
		{
			gint addr = 0, len = -1;
			guchar lf = 0;

			g_variant_get(request, "(iiy)", &addr, &len, &lf);

			text = lib3270_get_string_at_address(hSession, addr, len, lf);

		}
		break;

	case 4: // row, col, length, line-delimiter
		{
			guint row = 0, col = 0;
			gint len = -1;
			guchar lf = 0;

			g_variant_get(request, "(uuiy)", &row, &col, &len, &lf);

			text = lib3270_get_string_at(hSession, row, col, len, lf);

		}
		break;

	default:
		g_message("getstring was called with %u arguments.",(unsigned int) g_variant_n_children(request));
		return EINVAL;
	}

	if(!text)
		return errno;

	// Send response as UTF-8.
	g_autofree gchar * converted = g_convert_with_fallback(text,-1,"UTF-8",lib3270_get_display_charset(hSession),"?",NULL,NULL,error);
	ipc3270_response_append_string(response, converted);

	return 0;

}

