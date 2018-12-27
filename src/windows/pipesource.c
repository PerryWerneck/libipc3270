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

void ipc3270_wait_for_client(IPC3270_PIPE_SOURCE *source) {

	if(ConnectNamedPipe(source->hPipe,&source->overlap)) {
		g_message("Error %u in ConnectNamedPipe",(unsigned int) getLastError());
		// popup_lasterror("%s",_( "Error in ConnectNamedPipe" ));
		return;
	}

	switch(GetLastError()) {
	case ERROR_IO_PENDING:		// The overlapped connection in progress.
		source->state = PIPE_STATE_WAITING;
		break;


	case ERROR_PIPE_CONNECTED:	// Client is already connected, so signal an event.
		if(SetEvent(source->overlap.hEvent))
			break;

	default:
		g_message("Error %u in ConnectNamedPipe",(unsigned int) getLastError());
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


static gboolean IO_dispatch(GSource *source, GSourceFunc callback, gpointer data) {
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

			// popup_lasterror("%s", _( "Pipe connection failed" ));

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

static gboolean IO_closure(gpointer data) {
	return 0;
}

static void IO_finalize(GSource *source) {

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
