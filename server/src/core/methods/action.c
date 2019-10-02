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
#include <lib3270/actions.h>

int ipc3270_method_action(GObject *session, GVariant *request, GObject *response, GError G_GNUC_UNUSED(**error)) {

	if(g_variant_n_children(request) != 1) {
		g_message("action was called with %u arguments.",(unsigned int) g_variant_n_children(request));
		ipc3270_response_append_int32(response, EINVAL);
	}

	GVariant *value = g_variant_get_child_value(request,0);

	ipc3270_response_append_int32(response, lib3270_action_activate_by_name(g_variant_get_string(value,NULL), ipc3270_get_session(session)));

	g_variant_unref(value);

	return 0;
}

int ipc3270_method_activatable(GObject *session, GVariant *request, GObject *response, GError **error) {

	if(g_variant_n_children(request) != 1) {
		g_message("activatable was called with %u arguments.",(unsigned int) g_variant_n_children(request));

		if(!*error)
			g_set_error_literal(error,g_quark_from_static_string(PACKAGE_NAME),EINVAL,"Unexpected argument number");

		return 0;
	}

	GVariant *value = g_variant_get_child_value(request,0);

	const LIB3270_ACTION * action = lib3270_action_get_by_name(g_variant_get_string(value,NULL));

	if(action) {
		ipc3270_response_append_boolean(response, lib3270_action_is_activatable(action, ipc3270_get_session(session)));
	} else {
		ipc3270_response_append_boolean(response, FALSE);
	}

	g_variant_unref(value);

	return 0;
}
