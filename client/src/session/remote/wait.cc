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

#ifndef _WIN32
	#include <unistd.h>
#endif // _WIN32

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	void IPC::Session::wait(int seconds, std::function<int()> worker) const {

		int rc;

		time_t end = time(nullptr) + seconds;

		while(time(nullptr) < end) {

			rc = worker();

			debug("rc=",rc);

			if(rc == 0)
				return;
			else if(rc != ETIMEDOUT)
				chkResponse(rc);

		}

		chkResponse(ETIMEDOUT);

	}

	void IPC::Session::wait(time_t seconds) const {

		time_t end = time(nullptr) + seconds;

		while(time(nullptr) < end) {

#ifdef _WIN32
			Sleep(1000);
#else
			sleep(1);
#endif // _WIN32

			if(getConnectionState() == TN3270::DISCONNECTED)
				chkResponse(ENOTCONN);

		}

	}

	void IPC::Session::waitForReady(time_t timeout) const {

		debug(__FUNCTION__,"(",timeout,")");

		wait(timeout, [this]() {

			int rc;

			debug("Running waitForReady request...");

			Request(*this,"waitForReady")
				.push((uint32_t) 1)
				.call()
				.pop(rc);

			debug("Wait for ready returned ",rc);

			return rc;

		});

	}

	LIB3270_KEYBOARD_LOCK_STATE IPC::Session::waitForKeyboardUnlock(time_t timeout) const {

		int rc;

		time_t end = time(nullptr) + timeout;

		while(time(nullptr) < end) {

			debug("Running waitForKeyboardUnlock request...");

			Request(*this,"waitForKeyboardUnlock")
				.push((uint32_t) 1)
				.call()
				.pop(rc);

			debug("Wait for unlock returned ",rc);

			if(rc == 0)
				return (LIB3270_KEYBOARD_LOCK_STATE) 0;

		}

		return (LIB3270_KEYBOARD_LOCK_STATE) rc;

	}

	void IPC::Session::waitForChange(time_t seconds) const {

		wait(seconds, [this]() {

			int rc;
			debug("Running waitForUpdate request...");

			Request(*this,"waitForUpdate")
				.push((uint32_t) 1)
				.call()
				.pop(rc);

			debug("Wait for update returned ",rc);

			return rc;

		});

	}

	void IPC::Session::wait(const char *text, int seconds) {

		string key = convertToHost(text,-1);

		wait(seconds, [this, key]() {

			int rc;

			Request(*this,"waitForString")
				.push(key.c_str())
				.push((uint32_t) 1)
				.call()
				.pop(rc);

			return rc;

		});

	}

	void IPC::Session::wait(unsigned short row, unsigned short col, const char *text, int seconds) {

		string key = convertToHost(text,-1);

		wait(seconds, [this, key, row, col]() {

			int rc;

			Request(*this,"waitForStringAt")
				.push((uint32_t) row)
				.push((uint32_t) col)
				.push(key.c_str())
				.push((uint32_t) 1)
				.call()
				.pop(rc);

			return rc;

		});

	 }

	 void IPC::Session::wait(int addr, const char *text, int seconds) {

		string key = convertToHost(text,-1);

		wait(seconds, [this, key, addr]() {

			int rc;

			Request(*this,"waitForStringAtAddress")
				.push((int32_t) addr)
				.push(key.c_str())
				.push((uint32_t) 1)
				.call()
				.pop(rc);

			return rc;

		});

	 }

 }


