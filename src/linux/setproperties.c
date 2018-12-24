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
#include <lib3270/properties.h>

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>

gboolean
ipc3270_set_property (GDBusConnection  *connection,
                     const gchar      *sender,
                     const gchar      *object_path,
                     const gchar      *interface_name,
                     const gchar      *property_name,
                     GVariant         *value,
                     GError          **error,
                     gpointer          user_data)
{
	// Check for property
	size_t ix;

	const LIB3270_INT_PROPERTY * proplist = lib3270_get_int_properties_list();
	for(ix = 0; proplist[ix].name; ix++) {

		if(proplist[ix].set && !g_ascii_strcasecmp(proplist[ix].name, property_name)) {

			// Found it!
			if(proplist[ix].set(IPC3270(user_data)->hSession, (int) g_variant_get_int32(value))) {

				// Erro!
				g_set_error (error,
					G_IO_ERROR,
					G_IO_ERROR_FAILED,
					g_strerror(errno)
				);

				return FALSE;
			}

			return TRUE;

		}

	}

	// Check for toggle
	LIB3270_TOGGLE toggle = lib3270_get_toggle_id(property_name);
	if(toggle != (LIB3270_TOGGLE) -1) {

		// Is a Tn3270 toggle, get it!
		if(lib3270_set_toggle(IPC3270(user_data)->hSession,toggle,(int) g_variant_get_int32(value))) {

			// Erro!
			g_set_error (error,
				G_IO_ERROR,
				G_IO_ERROR_FAILED,
				g_strerror(errno)
			);

			return FALSE;

		}

		return TRUE;
	}

	// Check for pre-defineds
	if(!g_ascii_strcasecmp("url", property_name)) {
		lib3270_set_url(IPC3270(user_data)->hSession,g_variant_get_string(value,NULL));
		return TRUE;
	}

	g_set_error (error,
		G_IO_ERROR,
		G_IO_ERROR_NOT_FOUND,
		"Can't find any property named %s", property_name
	);

	return FALSE;
}
