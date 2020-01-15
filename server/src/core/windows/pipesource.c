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
 * Este programa está nomeado como pipesource.c e possui - linhas de código.
 *
 * Contatos:
 *
 * perry.werneck@gmail.com	(Alexandre Perry de Souza Werneck)
 * erico.mendonca@gmail.com	(Erico Mascarenhas Mendonça)
 *
 */

#include "gobject.h"
#include <ipc-glib.h>
#include <lib3270/trace.h>
#include <lib3270/toggle.h>
#include <lib3270/log.h>

void ipc3270_wait_for_client(IPC3270_PIPE_SOURCE *source) {

	if(ConnectNamedPipe(source->hPipe,&source->overlap)) {
		g_message("Error %u in ConnectNamedPipe",(unsigned int) GetLastError());
		// popup_lasterror("%s",_( "Error in ConnectNamedPipe" ));
		return;
	}

	switch(GetLastError()) {
	case ERROR_IO_PENDING:		// The overlapped connection in progress.
		source->state = PIPE_STATE_WAITING;
		break;

	case ERROR_PIPE_CONNECTED:	// Client is already connected, so signal an event.
		source->state = PIPE_STATE_WAITING;
		SetEvent(source->overlap.hEvent);
		break;

	default:
		g_message("Error %u in ConnectNamedPipe",(unsigned int) GetLastError());
		// popup_lasterror("%s", _( "ConnectNamedPipe failed" ));
	}

}

static gboolean IO_prepare(GSource *source, gint *timeout) {
	/*
 	 * Called before all the file descriptors are polled.
	 * If the source can determine that it is ready here
	 * (without waiting for the results of the poll() call)
	 * it should return TRUE.
	 *
	 * It can also return a timeout_ value which should be the maximum
	 * timeout (in milliseconds) which should be passed to the poll() call.
	 * The actual timeout used will be -1 if all sources returned -1,
	 * or it will be the minimum of all the timeout_ values
	 * returned which were >= 0.
	 *
	 */
	if(WaitForSingleObject(((IPC3270_PIPE_SOURCE *) source)->overlap.hEvent,0) == WAIT_OBJECT_0)
		return TRUE;

	*timeout = 10;

	return FALSE;

}

static gboolean IO_check(GSource *source) {
	/*
 	 * Called after all the file descriptors are polled.
 	 * The source should return TRUE if it is ready to be dispatched.
	 * Note that some time may have passed since the previous prepare
	 * function was called, so the source should be checked again here.
	 *
	 */
	if(WaitForSingleObject(((IPC3270_PIPE_SOURCE *) source)->overlap.hEvent,0) == WAIT_OBJECT_0)
		return TRUE;

	return FALSE;
}

static void process_input(IPC3270_PIPE_SOURCE *source, DWORD cbRead) {

	const gchar * request_name	= (const gchar *) (source->buffer);
	H3270		* hSession		= ipc3270_get_session(source->object);
	int			  request_type	= 0;

	if(lib3270_get_toggle(hSession,LIB3270_TOGGLE_EVENT_TRACE))
		lib3270_trace_data(hSession, "IPC Data block received on pipe", (const unsigned char *) source->buffer, (size_t) cbRead);

	debug("Received packet \"%s\" with %u bytes", request_name, (unsigned int) cbRead);

	g_autoptr (GError) error = NULL;
	g_autoptr (GVariant) response = NULL;
	g_autoptr (GVariant) parameters = ipc3270_unpack(source->buffer, &request_type);

	if(parameters) {

		// Process query
		switch(request_type) {
		case 1: // getProperty
			response = ipc3270_get_property(source->object, request_name, &error);
			break;

		case 2: // setProperty
			ipc3270_set_property(source->object, request_name, parameters, &error);
			response = g_variant_new_int32(0);
			break;

		case 3: // method
			{
				g_autoptr(GObject) rsp = ipc3270_response_new();

				debug("Parameters: %p", parameters);
				debug("rsp: %p", rsp);

				ipc3270_method_call(source->object, request_name, parameters, rsp, &error);

				if(ipc3270_response_has_values(rsp))
					response = ipc3270_response_steal_value(rsp);

			}
			break;

		default:
			g_message("Rejecting request \"%s\": Invalid type %d",request_name, request_type);
			g_set_error(&error,IPC3270(source->object)->error_domain,EINVAL,"Invalid or unexpected type %d",request_type);

		}

	} else if(!error) {

		g_set_error(&error,IPC3270(source->object)->error_domain,errno ? errno : EINVAL,"Can't parse parameter list");

	}

	debug("response=%p",response);

	// Pack response
	size_t szPacket = 0;
	g_autofree unsigned char * buffer = NULL;

	if(error) {

		buffer = ipc3270_pack_error(error, &szPacket);

	} else {

		buffer = ipc3270_pack_value(request_name, 0, response, &szPacket);

	}

	// Send response
	DWORD wrote = (DWORD) szPacket;

	if(lib3270_get_toggle(hSession,LIB3270_TOGGLE_EVENT_TRACE))
		lib3270_trace_data(hSession, "IPC Data block sent to pipe", (const unsigned char *) buffer, szPacket);

	WriteFile(source->hPipe,buffer,wrote,&wrote,NULL);

}

static void read_input_pipe(IPC3270_PIPE_SOURCE *source) {

	DWORD cbRead	= 0;

	if(ReadFile(source->hPipe,source->buffer,PIPE_BUFFER_LENGTH,&cbRead,&source->overlap) && cbRead > 0)
		process_input(source,cbRead);

	// The read operation is still pending.
	switch(GetLastError())
	{
	case 0:
		break;

	case ERROR_IO_PENDING:
		source->state = PIPE_STATE_PENDING_READ;
		break;

	case ERROR_PIPE_LISTENING:
		source->state = PIPE_STATE_READ;
		break;

	case ERROR_BROKEN_PIPE:

		if(!DisconnectNamedPipe(source->hPipe)) {

			g_message("Error %u on DisconnectNamedPipe", (unsigned int) GetLastError());

		} else {

			ipc3270_wait_for_client(source);

		}
		break;

	case ERROR_PIPE_NOT_CONNECTED:
		g_message("Cliente was disconnected");
		break;

	default:
		if(source->hPipe != INVALID_HANDLE_VALUE) {
			g_message( "Error %u receiving message from pipe", (unsigned int) GetLastError());
		}
	}

}


static gboolean IO_dispatch(GSource *source, GSourceFunc G_GNUC_UNUSED(callback), gpointer G_GNUC_UNUSED(data)) {
	/*
	 * Called to dispatch the event source,
	 * after it has returned TRUE in either its prepare or its check function.
	 * The dispatch function is passed in a callback function and data.
	 * The callback function may be NULL if the source was never connected
	 * to a callback using g_source_set_callback(). The dispatch function
	 * should call the callback function with user_data and whatever additional
	 * parameters are needed for this type of event source.
	 */
	BOOL	fSuccess;
	DWORD	cbRead	= 0;

	fSuccess = GetOverlappedResult(((IPC3270_PIPE_SOURCE *) source)->hPipe,&((IPC3270_PIPE_SOURCE *) source)->overlap,&cbRead,FALSE );

	switch(((IPC3270_PIPE_SOURCE *) source)->state) {
	case PIPE_STATE_WAITING:

		if(fSuccess) {

			g_message("Client is connected");
			((IPC3270_PIPE_SOURCE *) source)->state = PIPE_STATE_READ;

		} else {

			g_message("Pipe connection failed with rc=%u",(unsigned int) GetLastError());

		}
		break;

	case PIPE_STATE_READ:
		// trace("Reading pipe (cbRead=%d)",(int) cbRead);
		read_input_pipe( (IPC3270_PIPE_SOURCE *) source);
		break;

	case PIPE_STATE_PENDING_READ:
		if(fSuccess && cbRead > 0)
			process_input((IPC3270_PIPE_SOURCE *) source,cbRead);
		((IPC3270_PIPE_SOURCE *) source)->state = PIPE_STATE_READ;
		break;

	case PIPE_STATE_UNDEFINED:
		break;

	}

	return TRUE;
}

static gboolean IO_closure(gpointer source) {

	debug("%s(%p)",__FUNCTION__,source);

	return 0;
}

static void IO_finalize(GSource *source) {

	debug("%s(%p)",__FUNCTION__,source);

	if( ((IPC3270_PIPE_SOURCE *) source)->hPipe != INVALID_HANDLE_VALUE) {
		CloseHandle(((IPC3270_PIPE_SOURCE *) source)->hPipe);
		((IPC3270_PIPE_SOURCE *) source)->hPipe = INVALID_HANDLE_VALUE;
	}

}

GSourceFuncs ipc3270_source_funcs = {
	IO_prepare,
	IO_check,
	IO_dispatch,
	IO_finalize,
	IO_closure,
	NULL
};
