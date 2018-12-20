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
  */

#ifndef PRIVATE_H_INCLUDED

	#define PRIVATE_H_INCLUDED

	#define PACKAGE_NAME "pw3270"

	#define ENABLE_NLS
	#define GETTEXT_PACKAGE PACKAGE_NAME

	#include <libintl.h>
	#include <glib/gi18n.h>
	#include <gtk/gtk.h>
	#include <gio/gio.h>

	#include <v3270.h>

	G_BEGIN_DECLS

	#define GLIB_TYPE_IPC3270				(ipc3270_get_type ())
	#define IPC3270(obj)					(G_TYPE_CHECK_INSTANCE_CAST ((obj), GLIB_TYPE_IPC3270, ipc3270))
	#define IPC3270_CLASS(klass)			(G_TYPE_CHECK_CLASS_CAST ((klass), GLIB_TYPE_IPC3270, ipc3270Class))
	#define IS_IPC3270(obj)					(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GLIB_TYPE_IPC3270))
	#define IS_IPC3270_CLASS(klass)			(G_TYPE_CHECK_CLASS_TYPE ((klass), GLIB_TYPE_IPC3270))
	#define IPC3270_GET_CLASS(obj)			(G_TYPE_INSTANCE_GET_CLASS ((obj), GLIB_TYPE_IPC3270, ipc3270Class))

	typedef struct _ipc3270					ipc3270;
	typedef struct _ipc3270Class			ipc3270Class;

	GObject		* ipc3270_new(GtkWidget *window, GtkWidget *terminal);
	GType		  ipc3270_get_type(void);

	G_END_DECLS

	#ifdef DEBUG
		#define debug( fmt, ... )  fprintf(stderr,"%s(%d) " fmt "\n", __FILE__, (int) __LINE__, __VA_ARGS__ ); fflush(stderr);
	#else
		#define debug(...) /* __VA_ARGS */
	#endif

	int pw3270_plugin_start(GtkWidget *window, GtkWidget *terminal);
	int pw3270_plugin_stop(GtkWidget *window, GtkWidget *terminal);

#endif // PRIVATE_H_INCLUDED
