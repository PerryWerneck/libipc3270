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

#include <lib3270.h>
#include <ipc-glib.h>

/*--[ Implement ]------------------------------------------------------------------------------------*/

 #if ! GLIB_CHECK_VERSION(2,44,0)

 // Reference: https://github.com/ImageMagick/glib/blob/master/glib/glib-autocleanups.h
 void ipc3270_autoptr_cleanup_generic_gfree(void *p)
 {
        void **pp = (void**)p;
        g_free (*pp);
 }

 #endif // ! GLIB(2,44,0)

 const char * ipc3270_get_error_message(int errcode) {

	static const struct Messages {
		int errcode;
		const char *msg;
	} messages[] = {
		{ ENOTCONN, "Not connected to host" }
	};

	size_t ix;

	for(ix = 0; ix < G_N_ELEMENTS(messages); ix++) {
		if(messages[ix].errcode == errcode) {
			return messages[ix].msg;
		}
	}

	return strerror(errcode);
}

void ipc3270_set_error(GObject *object, int errcode, GError **error) {
	if(error && !*error) {
		g_set_error(error,ipc3270_get_error_domain(object),errcode,"%s",ipc3270_get_error_message(errcode));
	}
}


