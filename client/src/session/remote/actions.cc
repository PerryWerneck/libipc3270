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

	TN3270::Action * IPC::Session::getAction(const LIB3270_ACTION *descriptor) {
		return new IPC::Action(this, descriptor);
	}

	IPC::Action::Action(Session *session, const LIB3270_ACTION *descriptor) : TN3270::Action(descriptor) {
		this->session = session;
	}

	bool IPC::Action::activatable() const {

		bool rc;

		debug(__FUNCTION__);
		Request(*session,"activatable")
			.push(descriptor->name)
			.call()
			.pop(rc);
		debug(__FUNCTION__);

		return rc;
	}

	void IPC::Action::activate() {
		session->action(descriptor->name);
	}

	void IPC::Action::wait(time_t seconds) {
		session->waitForReady(seconds);
	}

	void IPC::Session::action(const char *action_name) {

		int32_t rc;

		Request(*this,"action")
			.push(action_name)
			.call()
			.pop(rc);

		if(rc) {
            throw std::system_error((int) rc, std::system_category());
		}

	}

 	void IPC::Session::connect(const char *url, int seconds) {

		if(!url)
			url = "";

		Request(*this,"connect")
			.push(url)
			.call();

		if(seconds)
			this->waitForReady(seconds);

	}

	void IPC::Session::disconnect() {

		Request(*this,"disconnect")
			.call();

	}

	void IPC::Session::pfkey(unsigned short value) {

		int32_t rc;

		Request(*this,"pfkey")
			.push((int32_t) value)
			.call()
			.pop(rc);

		if(rc) {
            throw std::system_error((int) rc, std::system_category());
		}

	}

	void IPC::Session::pakey(unsigned short value) {

		int32_t rc;

		Request(*this,"pakey")
			.push((int32_t) value)
			.call()
			.pop(rc);

		if(rc) {
            throw std::system_error((int) rc, std::system_category());
		}

	}

	void IPC::Session::push(const KeyboardAction action) {
		this->action(toCharString(action));
	}

	void IPC::Session::print(LIB3270_CONTENT_OPTION GNUC_UNUSED(option)) {
		throw std::system_error(ENOTSUP, std::system_category());
	}

 }


