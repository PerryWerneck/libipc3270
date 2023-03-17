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

	void Abstract::Session::wait(int seconds, const std::function<int32_t()> worker) const {

		int rc;

		time_t end = time(nullptr) + seconds;

		while(time(nullptr) < end) {

			rc = worker();

			debug("rc=",rc," (",strerror(rc),")");

			if(rc == 0)
				return;

			if(rc != ETIMEDOUT)
				chkResponse(rc);

		}

		throw runtime_error("Timeout");

	}

	void Abstract::Session::wait(time_t seconds) const {

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

	void Abstract::Session::waitForReady(time_t timeout) const {

		debug(__FUNCTION__,"(",timeout,")");

		wait(timeout, [this]() {

			debug("Running waitForReady request...");

			int32_t rc = RequestFactory(Request::Method,"waitForReady")->push((uint32_t) 1).get_int();

			debug("Wait for ready returned ",rc);

			return rc;

		});

	}

	void Abstract::Session::waitForConnected(time_t timeout) const {

		// TODO: Will be updated to lib3270_wait_for_connected at some point.
		return waitForConnectionState(CONNECTED_TN3270E,timeout);

	}

	void Abstract::Session::waitForConnectionState(ConnectionState state, time_t timeout) const {

		debug(__FUNCTION__,"(",timeout,")");

		wait(timeout, [this,state]() {

			debug("Running waitForConnectionState request...");

			int32_t rc = RequestFactory(Request::Method,"waitForConnectionState")->push((uint32_t) state, (uint32_t) 1).get_int();

			debug("Wait for connection state returned ",rc);

			return rc;

		});

	}

	LIB3270_KEYBOARD_LOCK_STATE Abstract::Session::waitForKeyboardUnlock(time_t timeout) const {

		int rc = 0;

		time_t end = time(nullptr) + timeout;

		while(time(nullptr) < end) {

			debug("Running waitForKeyboardUnlock request...");

			rc = RequestFactory(Request::Method,"waitForKeyboardUnlock")->push((uint32_t) 1).get_int();

			debug("Wait for unlock returned ",rc);

			if(rc == LIB3270_KL_UNLOCKED)
				break;

		}

		return (LIB3270_KEYBOARD_LOCK_STATE) rc;

	}

	void Abstract::Session::waitForChange(time_t seconds) const {

		wait(seconds, [this]() {

			debug("Running waitForUpdate request...");

			int32_t rc = RequestFactory(Request::Method,"waitForUpdate")->push((uint32_t) 1).get_int();

			debug("Wait for update returned ",rc);

			return rc;

		});

	}

	void Abstract::Session::wait(const char *text, int seconds) {

		string key = convertToHost(text,-1);

		wait(seconds, [this, key]() {

			return RequestFactory(Request::Method,"waitForString")->push(key.c_str(), (uint32_t) 1).get_int();

		});

	}

	void Abstract::Session::wait(uint32_t row, uint32_t col, const char *text, int seconds) {

		debug((const char *) __FUNCTION__, "(", row, ",", col, ",\"",text,"\")");

		string key = convertToHost(text,-1);

		wait(seconds, [this, key, row, col]() {

			int32_t rc = RequestFactory(Request::Method,"waitForStringAt")->push(row,col,key,(uint32_t) 1).get_int();
			debug("waitForStringAt rc=",rc);
			return rc;

		});

	 }

	 void Abstract::Session::wait(int32_t addr, const char *text, int seconds) {

		string key = convertToHost(text,-1);

		wait(seconds, [this, key, addr]() {

			return RequestFactory(Request::Method,"waitForStringAtAddress")->push(addr,key,(uint32_t) 1).get_int();

		});

	 }

 }


