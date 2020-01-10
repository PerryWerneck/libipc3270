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

#include <internals.h>
#include <ipc-glib.h>
#include <lib3270.h>
#include <lib3270/properties.h>
#include <lib3270/toggle.h>
#include <lib3270/trace.h>

GVariant * ipc3270_get_property(GObject *object, const gchar *property_name, GError **error) {

	size_t	  ix;
	H3270	* hSession = ipc3270_get_session(object);

	errno = 0; // Just in case.

	lib3270_write_event_trace(hSession,"GetProperty(%s) called on session %c\n",property_name,lib3270_get_session_id(hSession));

	// Boolean properties
	const LIB3270_INT_PROPERTY * boolprop = lib3270_get_boolean_properties_list();
	for(ix = 0; boolprop[ix].name; ix++) {

		if(boolprop[ix].get && !g_ascii_strcasecmp(boolprop[ix].name, property_name)) {

			// Found it!
			errno = 0;
			int value = boolprop[ix].get(hSession);

			debug("%s=%d (errno=%d)",property_name,value,errno);

			if(value > 0 || errno == 0) {
				return g_variant_new_boolean(value != 0);
			}

			// Erro!
			ipc3270_set_error(object,errno,error);
			return NULL;

		}

	}

	// int properties
	const LIB3270_INT_PROPERTY * intprop = lib3270_get_int_properties_list();
	for(ix = 0; intprop[ix].name; ix++) {

		if(intprop[ix].get && !g_ascii_strcasecmp(intprop[ix].name, property_name)) {

			// Found it!
			errno = 0; // Reset errno
			int value = intprop[ix].get(hSession);

			debug("%s=%d",property_name,value);

			if(errno == 0) {
				return g_variant_new_int32((gint32) value);
			}

			// Erro!
			ipc3270_set_error(object,errno,error);
			return NULL;

		}

	}

	// Unsigned int properties
	const LIB3270_UINT_PROPERTY * uintprop = lib3270_get_unsigned_properties_list();
	for(ix = 0; uintprop[ix].name; ix++) {

		if(uintprop[ix].get && !g_ascii_strcasecmp(uintprop[ix].name, property_name)) {

			// Found it!
			errno = 0; // Reset errno.
			unsigned int value = uintprop[ix].get(hSession);

			debug("%s=%d",property_name,value);

			if(errno == 0) {
				return g_variant_new_uint32((guint32) value);
			}

			// Erro!
			ipc3270_set_error(object,errno,error);
			return NULL;

		}

	}

	// String properties
	const LIB3270_STRING_PROPERTY * strprop = lib3270_get_string_properties_list();
	for(ix = 0; strprop[ix].name; ix++) {

		if(strprop[ix].get && !g_ascii_strcasecmp(strprop[ix].name, property_name)) {

			// Found it!
			const char * value = strprop[ix].get(hSession);

			if(value) {
				debug("%s=%s",property_name,value);
				return g_variant_new_string(value);
			}

			// Erro!
			ipc3270_set_error(object,errno,error);
			return NULL;

		}

	}

	// Check for toggle
	LIB3270_TOGGLE_ID toggle = lib3270_get_toggle_id(property_name);
	if(toggle != (LIB3270_TOGGLE_ID) -1) {

		// Is a Tn3270 toggle, get it!
		return g_variant_new_boolean(lib3270_get_toggle( (hSession), toggle) != 0);

	}

	return NULL;

}

