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
 * Este programa está nomeado como private.h e possui - linhas de código.
 *
 * Contatos:
 *
 * perry.werneck@gmail.com	(Alexandre Perry de Souza Werneck)
 *
 */

#ifndef PRIVATE_H_INCLUDED

	#define PRIVATE_H_INCLUDED
	#include <config.h>

	#define ERROR_DOMAIN g_quark_from_static_string(PACKAGE_NAME)

	#include <glib.h>
	#include <gio/gio.h>
	#include <lib3270.h>

	G_BEGIN_DECLS

	#define GLIB_TYPE_SESSION				(session_get_type ())
	#define SESSION(obj)					(G_TYPE_CHECK_INSTANCE_CAST ((obj), GLIB_TYPE_SESSION, session))
	#define SESSION_CLASS(klass)			(G_TYPE_CHECK_CLASS_CAST ((klass), GLIB_TYPE_SESSION, sessionClass))
	#define IS_SESSION(obj)					(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GLIB_TYPE_SESSION))
	#define IS_SESSION_CLASS(klass)			(G_TYPE_CHECK_CLASS_TYPE ((klass), GLIB_TYPE_SESSION))
	#define SESSION_GET_CLASS(obj)			(G_TYPE_INSTANCE_GET_CLASS ((obj), GLIB_TYPE_SESSION, sessionClass))

	typedef struct _session					session;
	typedef struct _sessionClass			sessionClass;

	G_GNUC_INTERNAL	void					service_start(void);
	G_GNUC_INTERNAL	GVariant * service_method_call(const gchar *method_name, GVariant *parameters, GError **error);
	G_GNUC_INTERNAL	GVariant * service_get_property(const gchar *property_name, GError **error);
	G_GNUC_INTERNAL	gboolean service_set_property(const gchar *property_name, GVariant *value, GError **error);

	G_END_DECLS

#endif // PRIVATE_H_INCLUDED
