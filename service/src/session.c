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

#include "private.h"
#include <ipc-glib.h>

struct _session {
	GObject			  parent;
	time_t			  activity;		///< @brief Timestamp da última atividade dessa sessão.
	time_t			  timeout;		///< @brief Após quantos segundos eu encerro a sessao?
	time_t			  maxidle;		///< @brief Tempo máximo que a sessão pode ficar IDLE
	time_t			  autoclose;	///< @brief Destroi a sessão quantos segundos após a desconexão?
};

struct _sessionClass {
	GObjectClass parent;
};

static GList * session_list = NULL;

G_DEFINE_TYPE(session, session, GLIB_TYPE_IPC3270)

static void session_finalize(GObject *object) {

	session_list = g_list_remove(session_list, object);

	lib3270_session_free(ipc3270_get_session(object));
	G_OBJECT_CLASS(session_parent_class)->finalize(object);

	debug("%s(%p)",__FUNCTION__,(void *) object);

}

static void session_class_init(sessionClass *klass) {

	debug("%s",__FUNCTION__);

	GObjectClass *object_class;
	object_class = G_OBJECT_CLASS (klass);
	object_class->finalize = session_finalize;

}

static void session_init(session *object) {

	debug("%s(%p)",__FUNCTION__,(void *) object);

	H3270 * hSession = lib3270_session_new("");
	lib3270_set_user_data(hSession,object);
	ipc3270_set_session(&object->parent,hSession);

	session_list = g_list_prepend(session_list, object);

}

GObject * session_new() {
	return g_object_new(GLIB_TYPE_SESSION, NULL);
}

