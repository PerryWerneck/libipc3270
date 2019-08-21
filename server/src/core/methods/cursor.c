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

int ipc3270_method_set_cursor(GObject *session, GVariant *request, GObject *response, GError **error) {

	H3270 *hSession = ipc3270_get_session(session);
	gint rc = -EINVAL;

	if(*error)
		return 0;

	switch(g_variant_n_children(request)) {
	case 1:	// Just only addr
		{
			gint addr;
			g_variant_get(request, "(i)", &addr);
			rc = lib3270_set_cursor_address(hSession,addr);
		}
		break;

	case 2:	// Row & Col
		{
			guint row, col;
			g_variant_get(request, "(uu)", &row, &col);
			rc = lib3270_set_cursor_position(hSession, row, col);
		}
		break;

	default:
		g_message("setcursor was called with %u arguments.",(unsigned int) g_variant_n_children(request));
		return EINVAL;
	}

	ipc3270_response_append_int32(response, rc);

	return 0;
}

