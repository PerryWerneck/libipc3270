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

#include "private.h"
#include <lib3270/actions.h>
#include <lib3270/trace.h>

int ipc3270_method_call(GObject *object, const gchar *method_name, GVariant *request, GObject *response, GError **error) {

	static const struct Methods {
			const char *name;
			int (*call)(GObject *session, GVariant *request, GObject *response, GError **error);
	} methods[] = {

		{ "connect", 					ipc3270_method_connect					},
		{ "disconnect",					ipc3270_method_disconnect				},

		{ "wait",						ipc3270_method_wait						},
		{ "waitforready",				ipc3270_method_wait_for_ready			},
		{ "waitforupdate",				ipc3270_method_wait_for_update			},

		{ "getString",					ipc3270_method_get_string				},
		{ "getStringAt",				ipc3270_method_get_string				},
		{ "getStringAtAddress",			ipc3270_method_get_string				},

		{ "setString",					ipc3270_method_set_string				},
		{ "setStringAt",				ipc3270_method_set_string				},
		{ "setStringAtAddress",			ipc3270_method_set_string				},

		{ "getFieldAttribute",			ipc3270_method_get_field_attribute		},
		{ "getFieldAttributeAt",		ipc3270_method_get_field_attribute		},
		{ "getFieldAttributeAtAddress",	ipc3270_method_get_field_attribute		},

	};

	size_t	  ix;
	H3270	* hSession = ipc3270_get_session(object);

	lib3270_trace_event(hSession,"Method %s called on session %c\n",method_name,lib3270_get_session_id(hSession));

	for(ix = 0; ix < G_N_ELEMENTS(methods); ix++) {

		if(!g_ascii_strcasecmp(methods[ix].name,method_name)) {

			int rc = methods[ix].call(object,request,response,error);

			if(rc)
				ipc3270_set_error(object,rc,error);

			return 0;
		}
	}

	// Check actions table.
	const LIB3270_ACTION_ENTRY * actions = lib3270_get_action_table();
	for(ix = 0; actions[ix].name; ix++) {

		if(!g_ascii_strcasecmp(actions[ix].name,method_name)) {

			int rc = actions[ix].call(hSession);
			if(rc)
			{
				// Failed
				ipc3270_set_error(object,errno,error);
				return;
 			}

			// Suceeded
			ipc3270_response_append_int32(response, 0);
			return 0;

		}
	}

	// Check lib3270 internal methods
	const IPC_METHOD_INT_ARG * int_methods = ipc3270_get_int_arg_methods();

	for(ix = 0; int_methods[ix].name; ix++) {

		if(!g_ascii_strcasecmp(int_methods[ix].name,method_name)) {

			gint value;
			g_variant_get(request, "(i)", &value);

			int rc = int_methods[ix].call(hSession, value);
			if(rc)
			{
				// Failed
				ipc3270_set_error(object,errno,error);
				return;
 			}

			// Suceeded
			ipc3270_response_append_int32(response, 0);
			return 0;

		}

	}

	g_message("Unknown method \"%s\"",method_name);
	return ENOENT;

}
