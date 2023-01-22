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
 #include <signal.h>

#ifndef _WIN32
	#include <unistd.h>
#endif // _WIN32

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	void Local::Session::wait(time_t seconds) const {

		handler->call([seconds](H3270 * hSession){
			return lib3270_wait(hSession, seconds);
		});

	}

	void Local::Session::waitForReady(time_t timeout) const {

		handler->call([timeout](H3270 * hSession){
			return lib3270_wait_for_ready(hSession, timeout);
		});

	}

	void Local::Session::waitForConnectionState(ConnectionState state, time_t timeout) const {

		handler->call([state,timeout](H3270 * hSession){
			return lib3270_wait_for_cstate(hSession, (LIB3270_CSTATE) state, timeout);
		});

	}

	LIB3270_KEYBOARD_LOCK_STATE Local::Session::waitForKeyboardUnlock(time_t timeout) const {

		std::lock_guard<std::mutex> lock(*handler);
		debug("waitForKeyboardUnlock(",timeout,")");
		return lib3270_wait_for_keyboard_unlock(handler->hSession, timeout);
	}

	void Local::Session::waitForChange(time_t seconds) const {

		handler->call([seconds](H3270 * hSession){
			debug("waitForUpdate(",seconds,")");
			return lib3270_wait_for_update(hSession, seconds);
		});

	}

	void Local::Session::wait(const char *text, int seconds) {

		handler->call([this,text,seconds](H3270 * hSession){
			return lib3270_wait_for_string(hSession,convertToHost(text,-1).c_str(),seconds);
		});

	}

	void Local::Session::wait(uint32_t row, uint32_t col, const char *text, int seconds) {

		handler->call([this,row,col,text,seconds](H3270 * hSession){
			return lib3270_wait_for_string_at(hSession,row,col,convertToHost(text,-1).c_str(),seconds);
		});

	}

	void Local::Session::wait(int addr, const char *text, int seconds) {

		handler->call([this,addr,text,seconds](H3270 * hSession){
			return lib3270_wait_for_string_at_address(hSession,addr,convertToHost(text,-1).c_str(),seconds);
		});

	}

 }


