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

/// @brief Converts lib3270 string to UTF-8 sets the method response.
void g_dbus_method_invocation_return_tn3270_string(ipc3270 *obj, GDBusMethodInvocation *invocation, char *string) {

	if(!string) {
		g_autoptr (GError) error = NULL;
		g_set_error(&error,obj->error_domain,errno,"%s",strerror(errno));
		g_dbus_method_invocation_return_gerror(invocation, error);
		return;
	}

	gchar * utftext = g_convert_with_fallback(string,-1,"UTF-8",lib3270_get_display_charset(obj->hSession),"?",NULL,NULL,NULL);

	g_dbus_method_invocation_return_value(invocation, g_variant_new ("(s)", utftext));

	lib3270_free(string);
	g_free(utftext);

}

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

	debug("%s(%s)",__FUNCTION__,method_name);

	if(!g_ascii_strcasecmp(method_name,"getString"))
	{
		g_dbus_method_invocation_return_tn3270_string(IPC3270(user_data), invocation, lib3270_get_string_at_address(IPC3270(user_data)->hSession,0,-1,'\n'));
		return;
	}
	else if(!g_ascii_strcasecmp(method_name,"setString"))
	{
		gchar *text = NULL;
		g_variant_get(parameters, "(&s)", &text);

		if(lib3270_input_string(IPC3270(user_data)->hSession,(const unsigned char *) text) < 0)
		{
			// Failed!
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
	else if(!g_ascii_strcasecmp(method_name,"setStringAt"))
	{
		gint row,col;
		gchar *text = NULL;
		g_variant_get(parameters, "(ii&s)", &row, &col, &text);

		if(lib3270_set_string_at(IPC3270(user_data)->hSession,row,col,(const unsigned char *) text) < 0)
		{
			// Failed!
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
	else if(!g_ascii_strcasecmp(method_name,"getStringAt"))
	{
		gint row,col,len;
		guchar lf;
		g_variant_get(parameters, "(iiy)", &row, &col, &len,&lf);

		g_dbus_method_invocation_return_tn3270_string(IPC3270(user_data), invocation, lib3270_get_string_at(IPC3270(user_data)->hSession, row, col, len, lf));
		return;

	}
	else if(!g_ascii_strcasecmp(method_name,"setStringAtAddress"))
	{
		gint addr;
		gchar *text = NULL;
		g_variant_get(parameters, "(i&s)", &addr, &text);

		if(lib3270_set_string_at_address(IPC3270(user_data)->hSession,addr,(unsigned char *) text) < 0)
		{
			// Failed!
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
	else if(!g_ascii_strcasecmp(method_name,"getStringAtAddress"))
	{
		gint addr,len;
		guchar lf;
		g_variant_get(parameters, "(iiy)", &addr, &len, &lf);

		g_dbus_method_invocation_return_tn3270_string(IPC3270(user_data), invocation, lib3270_get_string_at_address(IPC3270(user_data)->hSession, addr, len, lf));
		return;

	}
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

	for(ix = 0; int_methods[ix].name; ix++)
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
