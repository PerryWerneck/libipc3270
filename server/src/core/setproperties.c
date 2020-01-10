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

#include <config.h>
#include <ipc-glib.h>
#include <lib3270.h>
#include <lib3270/properties.h>
#include <lib3270/toggle.h>
#include <lib3270/trace.h>

gboolean ipc3270_set_property(GObject *object, const gchar *property_name, GVariant *value, GError **error) {

	// Check for property
	size_t	  ix;
	H3270	* hSession = ipc3270_get_session(object);

	if(!value) {
		g_set_error(error,ipc3270_get_error_domain(object),EINVAL,"Set property method requires an argument");
		return FALSE;
	}

	lib3270_write_event_trace(hSession,"SetProperty(%s) called on session %c\n",property_name,lib3270_get_session_id(hSession));

	// Boolean properties
	const LIB3270_INT_PROPERTY * boolprop = lib3270_get_boolean_properties_list();
	for(ix = 0; boolprop[ix].name; ix++) {

		if(boolprop[ix].set && !g_ascii_strcasecmp(boolprop[ix].name, property_name)) {

			// Found it!
			if(boolprop[ix].set(hSession, (int) (g_variant_get_boolean(value) ? 1 : 0))) {

				// Erro!
				g_set_error_literal(error,
					G_IO_ERROR,
					G_IO_ERROR_FAILED,
					g_strerror(errno)
				);

				return FALSE;
			}

			return TRUE;

		}

	}

	// Integer properties
	const LIB3270_INT_PROPERTY * intprop = lib3270_get_int_properties_list();
	for(ix = 0; intprop[ix].name; ix++) {

		if(intprop[ix].set && !g_ascii_strcasecmp(intprop[ix].name, property_name)) {

			// Found it!
			if(intprop[ix].set(hSession, (int) g_variant_get_int32(value))) {

				// Erro!
				g_set_error_literal(
					error,
					G_IO_ERROR,
					G_IO_ERROR_FAILED,
					g_strerror(errno)
				);

				return FALSE;
			}

			return TRUE;

		}

	}

	// Unsigned int properties
	const LIB3270_UINT_PROPERTY * uintprop = lib3270_get_unsigned_properties_list();
	for(ix = 0; uintprop[ix].name; ix++) {

		if(uintprop[ix].set && !g_ascii_strcasecmp(uintprop[ix].name, property_name)) {

			// Found it!
			if(uintprop[ix].set(hSession, (unsigned int) g_variant_get_uint32(value))) {

				// Erro!
				g_set_error_literal(
					error,
					G_IO_ERROR,
					G_IO_ERROR_FAILED,
					g_strerror(errno)
				);

				return FALSE;
			}

//			g_message("%s=%u",property_name,uintprop[ix].get(hSession));
			return TRUE;

		}

	}


	// String properties
	const LIB3270_STRING_PROPERTY * strprop = lib3270_get_string_properties_list();
	for(ix = 0; strprop[ix].name; ix++) {

		if(strprop[ix].set && !g_ascii_strcasecmp(strprop[ix].name, property_name)) {

			// Found it!
			if(strprop[ix].set(hSession, g_variant_get_string(value,NULL))) {

				// Erro!
				g_set_error_literal(error,
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
	LIB3270_TOGGLE_ID toggle = lib3270_get_toggle_id(property_name);
	if(toggle != (LIB3270_TOGGLE_ID) -1) {

		// Is a Tn3270 toggle, get it!
		if(lib3270_set_toggle(hSession,toggle,(int) (g_variant_get_boolean(value) ? 1 : 0))) {

			// Erro!
			g_set_error_literal(error,
				G_IO_ERROR,
				G_IO_ERROR_FAILED,
				g_strerror(errno)
			);

			return FALSE;

		}

		return TRUE;
	}

	g_set_error (error,
		G_IO_ERROR,
		G_IO_ERROR_NOT_FOUND,
		"Can't find any property named %s", property_name
	);

	return FALSE;
}
