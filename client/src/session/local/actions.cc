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
 * @file
 *
 * @brief
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include "private.h"
 #include <lib3270/actions.h>

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	void Local::Session::action(const char *action_name) {
		std::lock_guard<std::mutex> lock(sync);
		chkResponse(lib3270_action(hSession,action_name));
	}

 	void Local::Session::connect(const char *url, bool wait) {

		std::lock_guard<std::mutex> lock(sync);
		chkResponse(lib3270_connect_url(hSession,url,(wait ? 1 : 0)));
	}

	void Local::Session::disconnect() {

		std::lock_guard<std::mutex> lock(sync);
		chkResponse(lib3270_disconnect(hSession));
	}

	void Local::Session::wait(unsigned short seconds) const {

		std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);
		chkResponse(lib3270_wait(this->hSession, seconds));

	}

	void Local::Session::waitForReady(time_t timeout) const {

		std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);
		chkResponse(lib3270_wait_for_ready(this->hSession, timeout));

	}

	void Local::Session::waitForChange(unsigned short seconds) const {

		std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);
		chkResponse(lib3270_wait_for_update(this->hSession, seconds));

	}

	void Local::Session::pfkey(unsigned short value) {

		std::lock_guard<std::mutex> lock(sync);
		chkResponse(lib3270_pfkey(hSession,value));

	}

	void Local::Session::pakey(unsigned short value) {

		std::lock_guard<std::mutex> lock(sync);
		chkResponse(lib3270_pakey(hSession,value));

	}

	void Local::Session::push(const Action action) {

		typedef int (*ActionCallback)(H3270 *);

		static const ActionCallback actions[] = {
            lib3270_enter,			// ENTER
            lib3270_erase,			// ERASE
            lib3270_eraseeof,		// ERASE_EOF
            lib3270_eraseeol,		// ERASE_EOL
            lib3270_eraseinput,		// ERASE_INPUT
            lib3270_kybdreset,		// KYBD_RESET
            lib3270_newline,		// NEWLINE
            lib3270_clear,			// CLEAR
            lib3270_select_field,	// SELECT_FIELD
            lib3270_select_all,		// SELECT_ALL
            lib3270_unselect,		// UNSELECT
            lib3270_reselect,		// RESELECT
            lib3270_delete,			// DELETE
            lib3270_dup,			// DUP
            lib3270_fieldmark,		// FIELDMARK
            lib3270_backspace,		// BACKSPACE
            lib3270_previousword,	// WORD_PREVIOUS
            lib3270_nextword,		// WORD_NEXT
            lib3270_fieldend,		// FIELD_END
            lib3270_firstfield,		// FIELD_FIRST
            lib3270_nextfield,		// FIELD_NEXT
            lib3270_previousfield,	// FIELD_PREVIOUS
            lib3270_attn,			// ATTN
            lib3270_break,			// BREAK
            lib3270_deleteword,		// WORD_DELETE
            lib3270_deletefield,	// FIELD_DELETE
            lib3270_sysreq,			// SYSREQ
		};

		if( ((size_t) action) > (sizeof(actions)/sizeof(actions[0]))) {
            throw std::system_error(EINVAL, std::system_category());
		}

		std::lock_guard<std::mutex> lock(sync);
		chkResponse(actions[(size_t) action](hSession));

	}

	void Local::Session::print(LIB3270_CONTENT_OPTION option) {

		std::lock_guard<std::mutex> lock(sync);

		switch(option) {
		case LIB3270_CONTENT_ALL:
			chkResponse(lib3270_print_all(hSession));
			break;

		case LIB3270_CONTENT_COPY:
			chkResponse(lib3270_print_copy(hSession));
			break;

		case LIB3270_CONTENT_SELECTED:
			chkResponse(lib3270_print_selected(hSession));
			break;

		default:
            throw std::system_error(EINVAL, std::system_category());

		}

	}

 }


