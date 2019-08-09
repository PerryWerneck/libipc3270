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

/**
 * @file src/core/constants.cc
 *
 * @brief Translation & constant values.
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include <ipc-client-internals.h>


/*---[ Implement ]----------------------------------------------------------------------------------*/

TN3270_PUBLIC const char * toCharString(const TN3270::Action action) {

    static const char * actions[] = {
        "enter",
        "erase",
        "eraseeof",
        "eraseeol",
        "eraseinput"
        "kybdreset",
        "newline",
        "clear",
        "select_field",
        "select_all",
        "unselect",
        "reselect",
        "delete",
        "dup",
        "fieldmark",
        "backspace",
        "previousword",
        "nextword",
        "fieldend",
        "firstfield",
        "nextfield",
        "previousfield",
        "attn",
        "break",
        "deleteword",
        "deletefield",
        "sysreq"
    };

    if( ((size_t) action) > (sizeof(actions)/sizeof(actions[0]))) {
        throw std::system_error(EINVAL, std::system_category());
    }

    return actions[action];
}




