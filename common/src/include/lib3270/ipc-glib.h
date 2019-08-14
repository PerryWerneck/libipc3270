/*
 * "Software pw3270, desenvolvido com base nos códigos fontes do WC3270  e X3270
 * (Paul Mattes Paul.Mattes@usa.net), de emulação de terminal 3270 para acesso a
 * aplicativos mainframe. Registro no INPI sob o nome G3270. Registro no INPI sob
 * o nome G3270.
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

 /**
  * @brief Common definitions for pw3270 IPC plugin.
  *
  * @author Perry Werneck <perry.werneck@gmail.com>
  *
  */

#ifndef PW3270_IPC_H_INCLUDED

	#define PW3270_IPC_H_INCLUDED

	#ifdef _WIN32

		#define PW3270_IPC_SESSION_BUS_NAME					"\\\\.\\pipe\\%s\\%c"

	#else

		#define PW3270_IPC_SESSION_BUS_NAME			"br.com.bb.%s.%c"
		#define PW3270_IPC_SESSION_INTERFACE_NAME	"br.com.bb.tn3270.session"
		#define PW3270_IPC_SESSION_OBJECT_PATH		"/br/com/bb/tn3270/session"

		#define PW3270_IPC_SERVICE_BUS_NAME			"br.com.bb.tn3270.service"
		#define PW3270_IPC_SERVICE_INTERFACE_NAME	"br.com.bb.tn3270.service"
		#define PW3270_IPC_SERVICE_OBJECT_PATH		"/br/com/bb/tn3270/service"

	#endif // _WIN32

	#include <glib.h>
	#include <gtk/gtk.h>
	#include <lib3270.h>

	G_BEGIN_DECLS

	#define GLIB_TYPE_IPC3270_RESPONSE			(ipc3270Response_get_type ())
	#define IPC3270_RESPONSE(obj)				(G_TYPE_CHECK_INSTANCE_CAST ((obj), GLIB_TYPE_IPC3270_RESPONSE, ipc3270Response))
	#define IPC3270_RESPONSE_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), GLIB_TYPE_IPC3270_RESPONSE, ipc3270ResponseClass))
	#define IS_IPC3270_RESPONSE(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GLIB_TYPE_IPC3270_RESPONSE))
	#define IS_IPC3270_RESPONSE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GLIB_TYPE_IPC3270_RESPONSE))
	#define IPC3270_RESPONSE_GET_CLASS(obj)		G_TYPE_INSTANCE_GET_CLASS ((obj), GLIB_TYPE_IPC3270_RESPONSE, ipc3270ResponseClass))

	typedef struct _ipc3270Response				ipc3270Response;
	typedef struct _ipc3270ResponseClass		ipc3270ResponseClass;

	GObject		* ipc3270_response_new();

	void		  ipc3270_response_append_int32(GObject *object, gint32 value);
	void		  ipc3270_response_append_uint32(GObject *object, guint32 value);
	void		  ipc3270_response_append_string(GObject *object, const gchar *text);
	gboolean	  ipc3270_response_has_values(GObject *object);
	GVariant	* ipc3270_response_steal_value(GObject *object);


	#define GLIB_TYPE_IPC3270				(ipc3270_get_type ())
	#define IPC3270(obj)					(G_TYPE_CHECK_INSTANCE_CAST ((obj), GLIB_TYPE_IPC3270, ipc3270))
	#define IPC3270_CLASS(klass)			(G_TYPE_CHECK_CLASS_CAST ((klass), GLIB_TYPE_IPC3270, ipc3270Class))
	#define IS_IPC3270(obj)					(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GLIB_TYPE_IPC3270))
	#define IS_IPC3270_CLASS(klass)			(G_TYPE_CHECK_CLASS_TYPE ((klass), GLIB_TYPE_IPC3270))
	#define IPC3270_GET_CLASS(obj)			(G_TYPE_INSTANCE_GET_CLASS ((obj), GLIB_TYPE_IPC3270, ipc3270Class))

	typedef struct _ipc3270					ipc3270;
	typedef struct _ipc3270Class			ipc3270Class;

	GObject				* ipc3270_new();
	GType				  ipc3270_get_type(void);
    void				  ipc3270_set_terminal_widget(GObject *object, GtkWidget *widget);
    void				  ipc3270_export_object(GObject *object, const char *name, GError **error);

    /**
     * @brief Convert from native charset to lib3270 charset.
     *
     * @param object	ipc3270 Object.
     * @param string	UTF-8 string to convert.
     * @param error		Pointer to error object.
     *
     * @return Converted string in lib3270 charset.
     *
     */
	gchar * ipc3270_convert_to_3270(GObject *object, const gchar *string, GError **error);

	/**
	 * @brief Convert lib3270 received string to UTF-8.
	 *
	 * @param object	ipc3270 object.
	 * @param string	String received from lib3270.
     * @param error		Pointer to error object.
     *
     * @return The string converted to UTF-8.
     *
     */
	gchar * ipc3270_convert_from_3270(GObject *object, const gchar *string, GError **error);

	/**
	 * @brief Create a GVariant from a lib3270 string.
	 *
	 * @param object	ipc3270 object.
	 * @param string	String received from lib3270.
     * @param error		Pointer to error object.
     *
     * @return GVariant with the string converted to UTF-8.
     *
     */
	GVariant			* ipc3270_GVariant_from_input_string(GObject *object, char *string, GError **error);

	void				  ipc3270_add_terminal_introspection(GString *string);

	const gchar			* ipc3270_get_display_charset(GObject *object);
	H3270				* ipc3270_get_session(GObject *object);

	GQuark				  ipc3270_get_error_domain(GObject *object);

	void				  ipc3270_set_error(GObject *object, int errcode, GError **error);

	int					  ipc3270_method_call(GObject *object, const gchar *method_name, GVariant *request, GObject *response, GError **error);
	gboolean			  ipc3270_set_property(GObject *object, const gchar *property_name, GVariant *value, GError **error);
	GVariant			* ipc3270_get_property(GObject *object, const gchar *property_name, GError **error);

	#ifdef _WIN32
		unsigned char	* ipc3270_pack(const gchar *name, int id, GVariant *values, size_t * szPacket);
		unsigned char	* ipc3270_pack_value(const gchar *name, int id, GVariant *value, size_t * szPacket);
		unsigned char	* ipc3270_pack_error(const GError *error, size_t * szPacket);
		GVariant		* ipc3270_unpack(const unsigned char *packet, int *id);
	#endif // _WIN32

	G_END_DECLS

	#if ! GLIB_CHECK_VERSION(2,44,0)
        G_GNUC_INTERNAL void ipc3270_autoptr_cleanup_generic_gfree(void *p);
        #define g_autofree __attribute__((cleanup(ipc3270_autoptr_cleanup_generic_gfree)))
	#endif // ! GLIB(2,44,0)

	#ifdef DEBUG
		#define debug( fmt, ... )  fprintf(stderr,"%s(%d) " fmt "\n", __FILE__, (int) __LINE__, __VA_ARGS__ ); fflush(stderr);
	#else
		#define debug(...) /* __VA_ARGS */
	#endif

	typedef struct _ipc_method_int_arg {
		const gchar *name;
		int (*call)(H3270 *hSession, int keycode);
	} IPC_METHOD_INT_ARG;

	const IPC_METHOD_INT_ARG * ipc3270_get_int_arg_methods();

#endif // PW3270_IPC_H_INCLUDED
