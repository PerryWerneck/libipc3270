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

#include <ipc-client-internals.h>
#include <private/session.h>
#include <lib3270/ipc/field.h>

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	std::string	Abstract::Session::toString(int baddr, int len, char lf) const {
		return convertFromHost(get(baddr,len,lf));
	}

	std::string	Abstract::Session::toString(unsigned short row, unsigned short col, int len, char lf) const {
		return convertFromHost(get(row,col,len,lf));
	}

	/// @brief Get contents of field ad address.
	void Abstract::Session::pop(int baddr, std::string &text) {
		text = FieldFactory(baddr)->get();
	}

	/// @brief Get contents of field at position.
	void Abstract::Session::pop(unsigned short row, unsigned short col, std::string &text) {
		text = FieldFactory(row,col)->get();
	}

	/// @brief Get contents of field at cursor position.
	void Abstract::Session::pop(std::string &text) {
		text = FieldFactory()->get();
	}

 }


