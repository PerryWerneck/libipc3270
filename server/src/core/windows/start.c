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
 * Contatos:
 *
 * perry.werneck@gmail.com	(Alexandre Perry de Souza Werneck)
 * erico.mendonca@gmail.com	(Erico Mascarenhas Mendonça)
 *
 */

#include "gobject.h"
#include <lib3270.h>
#include <lib3270/actions.h>
#include <lib3270/properties.h>
#include <lib3270/log.h>
#include <ipc-glib.h>

void ipc3270_export_object(GObject *object, const char *name, GError G_GNUC_UNUSED(**error)) {

	char id;

	debug("%s object=%p name=%s type=%p",__FUNCTION__,object,name,ipc3270_get_type());

	ipc3270 * ipc = IPC3270(object);

	for(id='A';id < 'Z';id++) {

		gchar	* pipename = g_strdup_printf(PW3270_IPC_SESSION_BUS_NAME,name,id);
		gchar 	* ptr;
		HANDLE	  hPipe;

		for(ptr=pipename;*ptr;ptr++)
			*ptr = g_ascii_tolower(*ptr);

		hPipe = CreateNamedPipe(	TEXT(pipename),				// pipe name
									PIPE_ACCESS_DUPLEX |		// read/write access
									FILE_FLAG_OVERLAPPED,		// overlapped mode
									PIPE_TYPE_MESSAGE |			// pipe type
									PIPE_READMODE_MESSAGE |		// pipe mode
									PIPE_WAIT,					// blocking mode
									1,							// number of instances
									PIPE_BUFFER_LENGTH,   		// output buffer size
									PIPE_BUFFER_LENGTH,			// input buffer size
									NMPWAIT_USE_DEFAULT_WAIT,	// client time-out
									NULL);						// default security attributes

		debug("%s = %p",pipename,hPipe);
		g_free(pipename);

		if(hPipe != INVALID_HANDLE_VALUE) {

			ipc->source = (IPC3270_PIPE_SOURCE *) g_source_new(&ipc3270_source_funcs,sizeof(IPC3270_PIPE_SOURCE));

			g_message("Got session \"%c\"",id);

			lib3270_set_session_id(ipc->hSession, id);

			ipc->source->hPipe			= hPipe;
			ipc->source->object			= object;
			ipc->source->state			= PIPE_STATE_WAITING;
			ipc->source->overlap.hEvent	= CreateEvent( NULL,TRUE,TRUE,NULL);

			g_source_attach((GSource *) ipc->source,NULL);

			//  IO_accept(source);
			ipc3270_wait_for_client(ipc->source);

			break;
		}

	}

}
