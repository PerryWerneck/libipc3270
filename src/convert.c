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
 * Este programa está nomeado como main.c e possui - linhas de código.
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

#include <lib3270.h>
#include <lib3270/actions.h>
#include <lib3270/ipc.h>

gchar * ipc3270_convert_output_string(GObject *object, const gchar *string, GError **error) {
	return g_convert_with_fallback(string,-1,ipc3270_get_display_charset(object),"UTF-8","?",NULL,NULL,error);
}

gchar * ipc3270_convert_input_string(GObject *object, const gchar *string, GError **error) {
	return g_convert_with_fallback(string,-1,"UTF-8",ipc3270_get_display_charset(object),"?",NULL,NULL,error);
}

GVariant * ipc3270_GVariant_from_input_string(GObject *object, char *string, GError **error) {

	if(string) {
		g_autofree gchar * utfstring = ipc3270_convert_input_string(object,string,error);
		lib3270_free(string);
		return g_variant_new("(s)", utfstring);
	}

	ipc3270_set_error(object,errno,error);
	return g_variant_new("(s)", "");
}


