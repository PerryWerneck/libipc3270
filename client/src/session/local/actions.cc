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

 }


