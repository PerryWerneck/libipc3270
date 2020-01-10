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

		{ "wait",						ipc3270_method_wait							},
		{ "waitforready",				ipc3270_method_wait_for_ready				},
		{ "waitforupdate",				ipc3270_method_wait_for_update				},
		{ "waitforkeyboardunlock",		ipc3270_method_wait_for_keyboard_unlock		},

		{ "getString",					ipc3270_method_get_string					},
		{ "getStringAt",				ipc3270_method_get_string					},
		{ "getStringAtAddress",			ipc3270_method_get_string					},

		{ "setString",					ipc3270_method_set_string					},
		{ "setStringAt",				ipc3270_method_set_string					},
		{ "setStringAtAddress",			ipc3270_method_set_string					},

		{ "waitForString",				ipc3270_method_wait_for_string				},
		{ "waitForStringAt",			ipc3270_method_wait_for_string				},
		{ "waitForStringAtAddress",		ipc3270_method_wait_for_string				},

		{ "setField",					ipc3270_method_set_field_contents			},
		{ "getFieldAttribute",			ipc3270_method_get_field_attribute			},
		{ "getFieldAttributeAt",		ipc3270_method_get_field_attribute			},
		{ "getFieldAttributeAtAddress",	ipc3270_method_get_field_attribute			},

		{ "setCursorAddress",			ipc3270_method_set_cursor					},
		{ "setCursorPosition",			ipc3270_method_set_cursor					},

		{ "action",						ipc3270_method_action						},
		{ "activatable",				ipc3270_method_activatable					},

	};

	size_t	  ix;
	H3270	* hSession = ipc3270_get_session(object);

	debug("%s(%s,request=%p,response=%p)",__FUNCTION__,method_name,request,response);

	lib3270_write_event_trace(hSession,"Method %s called on session %c\n",method_name,lib3270_get_session_id(hSession));

	for(ix = 0; ix < G_N_ELEMENTS(methods); ix++) {

		if(!g_ascii_strcasecmp(methods[ix].name,method_name)) {

			int rc = methods[ix].call(object,request,response,error);

			debug("rc=%d",rc);

			if(rc)
				ipc3270_set_error(object,rc,error);

			return 0;
		}
	}

	// Check actions table.
	const LIB3270_ACTION * action = lib3270_action_get_by_name(method_name);
	if(action) {
		if(lib3270_action_activate(action,hSession)) {
			ipc3270_set_error(object,errno,error);
		}
        return 0;
	}

	// Check lib3270 internal methods
	const IPC_METHOD_INT_ARG * int_methods = ipc3270_get_int_arg_methods();

	for(ix = 0; int_methods[ix].name; ix++) {

		if(!g_ascii_strcasecmp(int_methods[ix].name,method_name)) {

			gint value;
			g_variant_get(request, "(i)", &value);

			if(int_methods[ix].call(hSession, value))
				ipc3270_set_error(object,errno,error);
			else
				ipc3270_response_append_int32(response, 0);

			return 0;

		}

	}

	g_message("Unknown method \"%s\"",method_name);
	return ENOENT;

}
