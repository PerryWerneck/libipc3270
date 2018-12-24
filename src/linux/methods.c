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

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>

void
ipc3270_method_call (GDBusConnection       *connection,
                    const gchar           *sender,
                    const gchar           *object_path,
                    const gchar           *interface_name,
                    const gchar           *method_name,
                    GVariant              *parameters,
                    GDBusMethodInvocation *invocation,
                    gpointer               user_data)
{


	size_t ix;
	g_autoptr (GError) error = NULL;

	// Check action table.
	const LIB3270_ACTION_ENTRY * actions = lib3270_get_action_table();
	for(ix = 0; actions[ix].name; ix++)
	{
		if(!g_ascii_strcasecmp(actions[ix].name,method_name)) {

			int rc = actions[ix].call(IPC3270(user_data)->hSession);
			if(rc)
			{
				// Failed
				g_set_error(&error,IPC3270(user_data)->error_domain,errno,"%s: %s",method_name,strerror(errno));
				g_dbus_method_invocation_return_gerror(invocation, error);
 			}
 			else
			{
				// Suceeded
				g_dbus_method_invocation_return_value (invocation, g_variant_new_int16((gint16) 0));

			}

			return;
		}
	}

	// Check int methods
	const IPC_METHOD_INT_ARG * int_methods = ipc3270_get_int_arg_methods();

	for(ix = 0; ix < G_N_ELEMENTS(int_methods); ix++)
	{
		if(!g_ascii_strcasecmp(int_methods[ix].name,method_name)) {

			gint value;
			g_variant_get(parameters, "(i)", &value);

			int rc = int_methods[ix].call(IPC3270(user_data)->hSession, value);
			if(rc)
			{
				// Failed
				g_set_error(&error,IPC3270(user_data)->error_domain,errno,"%s: %s",method_name,strerror(errno));
				g_dbus_method_invocation_return_gerror(invocation, error);
 			}
 			else
			{
				// Suceeded
				g_dbus_method_invocation_return_value (invocation, g_variant_new_int16((gint16) 0));

			}

			return;

		}

	}

	g_dbus_method_invocation_return_error (
		invocation,
		G_DBUS_ERROR,
		G_DBUS_ERROR_UNKNOWN_METHOD,
		"Invalid or unexpected method call");

}
