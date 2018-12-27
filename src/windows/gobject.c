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
 * Este programa está nomeado como gobject.c e possui - linhas de código.
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

G_DEFINE_TYPE(ipc3270, ipc3270, G_TYPE_OBJECT)

static void ipc3270_finalize(GObject *object) {

	ipc3270 * ipc = IPC3270(object);


	G_OBJECT_CLASS(ipc3270_parent_class)->finalize(object);
}


static void ipc3270_class_init(ipc3270Class *klass) {

	debug("%s",__FUNCTION__);

	GObjectClass *object_class;
	object_class = G_OBJECT_CLASS (klass);
	object_class->finalize = ipc3270_finalize;

}

static void ipc3270_init(ipc3270 *object) {

	debug("%s",__FUNCTION__);
	object->error_domain = g_quark_from_static_string(PACKAGE_NAME);

}

GObject * ipc3270_new() {
	return g_object_new(GLIB_TYPE_IPC3270, NULL);
}

void ipc3270_set_session(GObject *object, H3270 *hSession, const char *name, GError **error) {

	char id;

	ipc3270 * ipc = IPC3270(object);
	ipc->hSession = hSession;

	for(id='A';id < 'Z';id++) {

		gchar	* pipename = g_strdup_printf("\\\\.\\pipe\\%s\\%c",name,id);
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

			ipc->source = (IPC3270_PIPE_SOURCE *) g_source_new(&pipe_source_funcs,sizeof(IPC3270_PIPE_SOURCE));

			lib3270_set_session_id(ipc->hSession, id);

			ipc->source->hPipe			= hPipe;
			ipc->source->state			= PIPE_STATE_WAITING;
			ipc->source->overlap.hEvent	= CreateEvent( NULL,TRUE,TRUE,NULL);

			g_source_attach((GSource *) ipc->source,NULL);

			//  IO_accept(source);
			ipc3270_wait_for_client(ipc->source);

			break;
		}

	}


}

const gchar * ipc3270_get_display_charset(GObject *object) {
	return lib3270_get_display_charset(IPC3270(object)->hSession);
}

