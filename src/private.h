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
  * @brief Private definitions for pw3270 IPC plugin.
  *
  */

#ifndef PRIVATE_H_INCLUDED

	#define PRIVATE_H_INCLUDED

	#define PACKAGE_NAME "pw3270"

	#include <libintl.h>
	#include <glib/gi18n.h>
	#include <gtk/gtk.h>
	#include <gio/gio.h>

	#include <v3270.h>

	#ifdef _WIN32

		#include <windows.h>

	#else

		G_GNUC_INTERNAL struct DBusSession {
			GDBusConnection	* connection;
			GDBusProxy		* proxy;
		} dBus;

	#endif // _WIN32

	#ifdef DEBUG
		#define debug( fmt, ... )  fprintf(stderr,"%s(%d) " fmt "\n", __FILE__, (int) __LINE__, __VA_ARGS__ ); fflush(stderr);
	#else
		#define debug(...) /* __VA_ARGS */
	#endif


	int pw3270_plugin_start(GtkWidget *window, GtkWidget *terminal);
	int pw3270_plugin_stop(GtkWidget *window, GtkWidget *terminal);

#endif // PRIVATE_H_INCLUDED
