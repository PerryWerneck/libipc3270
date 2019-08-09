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
 * @file src/core/session.cc
 *
 * @brief Implements common session object.
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include <ipc-client-internals.h>


/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	/// @brief Create a tn3270 session.
	Session * Session::create(const char *id) {

		debug("Creating session with ID \"",id,"\"");

		if(!(id && *id)) {
			return new Local::Session();
		}

		return new IPC::Session(id);

	}


	Session::Session() {

	}

	Session::~Session() {

	}

	void Session::insert(Event::Type type, std::function <void(const Event &event)> listener) {
	}

	/// @brief Fire event.
	void Session::fire(const Event &event) {


	}

	Session & Session::push(const PFKey key) {
		return pfkey( ((unsigned short) key) + 1);
	}

	Session & Session::push(const PAKey key) {
		return pakey( ((unsigned short) key) + 1);
	}

	Session & Session::push(int row, int col, const std::string &text) {
		return push(row,col,text.c_str(),text.size());
	}

	Session & Session::push(int baddr, const std::string &text) {
		return push(baddr,text.c_str(),text.size());
	}

	void Session::setCharSet(const char *charset) {
	}



 }



