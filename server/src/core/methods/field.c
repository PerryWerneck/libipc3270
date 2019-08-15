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

int ipc3270_method_get_field_attribute(GObject *session, GVariant *request, GObject *response, GError G_GNUC_UNUSED(**error)) {

	debug("%s childs=%u",__FUNCTION__,(unsigned int) g_variant_n_children(request));


	H3270 *hSession = ipc3270_get_session(session);
	guint row = 0, col = 0;
	gint baddr = -1;

	switch(g_variant_n_children(request)) {
	case 0: // No arguments, get at the current cursor position
		baddr = -1;
		break;

	case 1: // address
		g_variant_get(request, "(i)", &baddr);
		break;

	case 2:	// row, col
		g_variant_get(request, "(ii)", &row, &col);
		baddr = lib3270_translate_to_address(hSession,row,col);
		break;

	default:
		return EINVAL;
	}

	LIB3270_FIELD_ATTRIBUTE attr = lib3270_get_field_attribute(hSession,baddr);
	if(attr == LIB3270_FIELD_ATTRIBUTE_INVALID)
		return errno;

	ipc3270_response_append_uint32(response, (guint32) attr);

	return 0;
}

