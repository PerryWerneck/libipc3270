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
  * @brief Private definitions for pw3270 IPC windows module.
  *
  */

#ifndef WINDOWS_GOBJECT_H_INCLUDED

	#define WINDOWS_GOBJECT_H_INCLUDED

	#include <winsock2.h>
	#include <windows.h>
	#include <config.h>

	#define ENABLE_NLS

	#include <libintl.h>
	#include <glib/gi18n-lib.h>
	#include <gio/gio.h>

	#include <lib3270.h>
	#include <ipc-glib.h>

	#define PIPE_BUFFER_LENGTH 8192

	G_BEGIN_DECLS

	typedef struct _ipc3270			ipc3270;
	typedef struct _ipc3270Class	ipc3270Class;
	typedef struct _ipc3270Request	ipc3270Request;

	typedef enum _ipc3270_pipe_state {
		PIPE_STATE_WAITING,
		PIPE_STATE_READ,
		PIPE_STATE_PENDING_READ,
		PIPE_STATE_UNDEFINED
	} IPC3270_PIPE_STATE;

	typedef struct _ipc3270_pipe_source {
		GSource 			  gsrc;
		GObject 			* object;
		HANDLE				  hPipe;

		IPC3270_PIPE_STATE	  state;

		OVERLAPPED			  overlap;
		unsigned char		  buffer[PIPE_BUFFER_LENGTH+1];

	} IPC3270_PIPE_SOURCE;

	struct _ipc3270 {
		GObject			  	  parent;
		H3270				* hSession;
		GtkWidget			* terminal;
		gchar				* charset;
		IPC3270_PIPE_SOURCE	* source;
		GQuark 				  error_domain;
	};

	struct _ipc3270Class {
		GObjectClass parent;
	};

	G_GNUC_INTERNAL GSourceFuncs	  ipc3270_source_funcs;

	G_GNUC_INTERNAL void 			  ipc3270_release_object(ipc3270 *object);
	G_GNUC_INTERNAL void			  ipc3270_wait_for_client(IPC3270_PIPE_SOURCE *source);

	G_END_DECLS

#endif // WINDOWS_GOBJECT_H_INCLUDED
