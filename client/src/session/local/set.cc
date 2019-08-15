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

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

 	void Local::Session::set(const std::string &str) {

		std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);
		chkResponse(lib3270_input_string(hSession,(unsigned char *) str.c_str(),str.length()));

	}

	void Local::Session::set(int baddr, const std::string &str) {

		std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);
		chkResponse(lib3270_set_string_at_address(hSession,baddr,(unsigned char *) str.c_str(),str.length()));

	}

	void Local::Session::set(int row, int col, const std::string &str) {

		std::lock_guard<std::mutex> lock(const_cast<Local::Session *>(this)->sync);
		chkResponse(lib3270_set_string_at(hSession,row,col,(unsigned char *) str.c_str(),str.length()));

	}

 }


