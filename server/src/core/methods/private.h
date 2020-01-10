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
 * Referências:
 *
 * https://github.com/joprietoe/gdbus/blob/master/gdbus-example-server.c
 * https://github.com/bratsche/glib/blob/master/gio/tests/gdbus-example-export.c
 *
 * Contatos:
 *
 * perry.werneck@gmail.com	(Alexandre Perry de Souza Werneck)
 * erico.mendonca@gmail.com	(Erico Mascarenhas Mendonça)
 *
 */

#ifndef IPC_METHODS_H_INCLUDED

	#define IPC_METHODS_H_INCLUDED

	#include <internals.h>
	#include <lib3270.h>
	#include <ipc-glib.h>

	G_GNUC_INTERNAL int ipc3270_method_connect(GObject *session, GVariant *request, GObject *response, GError **error);
	G_GNUC_INTERNAL int ipc3270_method_disconnect(GObject *session, GVariant *request, GObject *response, GError **error);

	G_GNUC_INTERNAL int ipc3270_method_get_string(GObject *session, GVariant *request, GObject *response, GError **error);

	G_GNUC_INTERNAL int ipc3270_method_set_string(GObject *session, GVariant *request, GObject *response, GError **error);
	G_GNUC_INTERNAL int ipc3270_method_set_field_contents(GObject *session, GVariant *request, GObject *response, GError **error);

	G_GNUC_INTERNAL int ipc3270_method_wait_for_string(GObject *session, GVariant *request, GObject *response, GError **error);

	G_GNUC_INTERNAL int ipc3270_method_wait(GObject *session, GVariant *request, GObject *response, GError **error);
	G_GNUC_INTERNAL int ipc3270_method_wait_for_ready(GObject *session, GVariant *request, GObject *response, GError **error);
	G_GNUC_INTERNAL int ipc3270_method_wait_for_update(GObject *session, GVariant *request, GObject *response, GError **error);
	G_GNUC_INTERNAL int ipc3270_method_wait_for_keyboard_unlock(GObject *session, GVariant *request, GObject *response, GError **error);

	G_GNUC_INTERNAL int ipc3270_method_set_cursor(GObject *session, GVariant *request, GObject *response, GError **error);

	G_GNUC_INTERNAL int ipc3270_method_action(GObject *session, GVariant *request, GObject *response, GError **error);
	G_GNUC_INTERNAL int ipc3270_method_activatable(GObject *session, GVariant *request, GObject *response, GError **error);

	G_GNUC_INTERNAL int ipc3270_method_get_field_attribute(GObject *session, GVariant *request, GObject *response, GError **error);

#endif // IPC_METHODS_H_INCLUDED

