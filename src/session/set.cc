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

 #ifdef HAVE_CONFIG_H
	#include <config.h>
 #endif // HAVE_CONFIG_H

 #include <private/session.h>
 #include <lib3270/ipc/request.h>
 #include <ipc-client-internals.h>
 #include <cstring>

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	void Abstract::Session::push(const char *text, size_t length) {
		set(convertToHost(text,length));
	}

	void Abstract::Session::push(int baddr, const char *text, int length) {
		set(baddr,convertToHost(text,length));
	}

	void Abstract::Session::push(unsigned short row, unsigned short col, const char *text, int length) {
		set(row,col,convertToHost(text,length));
	}

	void Abstract::Session::setUnlockDelay(unsigned short delay) {
		setProperty<uint32_t>("unlock_delay",delay);
	}

	void Abstract::Session::setLockOnOperatorError(bool lock) {
		setProperty<uint32_t>("oerrlock",lock);
	}

	unsigned short Abstract::Session::setCursor(int addr) {

		int32_t rc;
		RequestFactory(Request::Method,"setCursorAddress")->push(addr).call().pop(rc);

		if(rc < 0)
			chkResponse(-rc);

		return (unsigned short) rc;

	}

	void Abstract::Session::setTimeout(time_t timeout) {

		int32_t rc;
		RequestFactory(Request::Method,"setWaitMode")->push( (int32_t) (timeout != 0) ).call().pop(rc);
		chkResponse(rc);

	}

 }


