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

#include <config.h>
#include <string.h>
#include <lib3270/ipc.h>
#include <lib3270.h>
#include <lib3270/actions.h>
#include <lib3270/trace.h>

// #include <dbus/dbus-glib.h>
// #include <dbus/dbus-glib-bindings.h>

GVariant * ipc3270_method_call(GObject *object, const gchar *method_name, GVariant *parameters, GError **error)
{

	size_t ix;
	H3270	* hSession = ipc3270_get_session(object);

	lib3270_trace_event(hSession,"Method %s called on session %c\n",method_name,lib3270_get_session_id(hSession));

	debug("%s(%s)",__FUNCTION__,method_name);

	if(!g_ascii_strcasecmp(method_name,"getString"))
	{
		return ipc3270_GVariant_from_input_string(object, lib3270_get_string_at_address(hSession,0,-1,'\n'), error);
	}
	else if(!g_ascii_strcasecmp(method_name,"setString"))
	{
		gchar *text = NULL;
		g_variant_get(parameters, "(&s)", &text);

		g_autofree gchar * converted = ipc3270_convert_output_string(object, text, error);
		if(lib3270_input_string(hSession,(const unsigned char *) converted) < 0)
		{
			// Failed!
			debug("%s failed: %s",method_name,strerror(errno));
			ipc3270_set_error(object,errno,error);
			return NULL;
		}

		// Suceeded
		debug("%s Suceeds",method_name);
		return g_variant_new("(i)", (gint) 0);

	}
	else if(!g_ascii_strcasecmp(method_name,"setStringAt"))
	{
		guint row,col;
		gchar *text = NULL;
		g_variant_get(parameters, "(ii&s)", &row, &col, &text);

		g_autofree gchar * converted = ipc3270_convert_output_string(object, text, error);
		if(lib3270_set_string_at(hSession,row,col,(const unsigned char *) converted) < 0)
		{
			// Failed!
			ipc3270_set_error(object,errno,error);
			return NULL;
		}

		// Suceeded
		return g_variant_new("(i)", (gint) 0);

	}
	else if(!g_ascii_strcasecmp(method_name,"getStringAt"))
	{
		guint row,col,len;
		guchar lf;
		g_variant_get(parameters, "(uuuy)", &row, &col, &len,&lf);

		return ipc3270_GVariant_from_input_string(object,lib3270_get_string_at(hSession, row, col, len, lf),error);

	}
	else if(!g_ascii_strcasecmp(method_name,"setStringAtAddress"))
	{
		guint addr;
		gchar *text = NULL;
		g_variant_get(parameters, "(i&s)", &addr, &text);

		g_autofree gchar * converted = ipc3270_convert_output_string(object, text, error);
		if(lib3270_set_string_at_address(hSession,addr,(unsigned char *) converted) < 0)
		{
			// Failed!
			ipc3270_set_error(object,errno,error);
			return NULL;
		}

		// Suceeded
		return g_variant_new("(i)", (gint) 0);

	}
	else if(!g_ascii_strcasecmp(method_name,"getStringAtAddress"))
	{
		guint addr,len;
		guchar lf;
		g_variant_get(parameters, "(uuy)", &addr, &len, &lf);

		debug("lf=%02x",(int) lf);

		return ipc3270_GVariant_from_input_string(object,lib3270_get_string_at_address(hSession, addr, len, lf),error);

	}
	else if(!g_ascii_strcasecmp(method_name,"waitforready"))
	{
		guint timeout = 1;
		g_variant_get(parameters, "(u)", &timeout);
		return g_variant_new("(i)", (gint) lib3270_wait_for_ready(hSession,timeout));
	}

	// Check action table.
	const LIB3270_ACTION_ENTRY * actions = lib3270_get_action_table();
	for(ix = 0; actions[ix].name; ix++)
	{
		if(!g_ascii_strcasecmp(actions[ix].name,method_name)) {

			int rc = actions[ix].call(hSession);
			if(rc)
			{
				// Failed
				ipc3270_set_error(object,errno,error);
				return NULL;
 			}

			// Suceeded
			return g_variant_new("(i)", (gint) 0);

		}
	}

	// Check int methods
	const IPC_METHOD_INT_ARG * int_methods = ipc3270_get_int_arg_methods();

	for(ix = 0; int_methods[ix].name; ix++)
	{
		if(!g_ascii_strcasecmp(int_methods[ix].name,method_name)) {

			gint value;
			g_variant_get(parameters, "(i)", &value);

			int rc = int_methods[ix].call(hSession, value);
			if(rc)
			{
				// Failed
				ipc3270_set_error(object,errno,error);
				return NULL;
 			}

			// Suceeded
			return g_variant_new("(i)", (gint) 0);

		}

	}

	g_message("Unknown method \"%s\"",method_name);

	return NULL;

}
