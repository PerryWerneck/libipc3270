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

 	void IPC::Session::set(const std::string &str) {

		int rc;

		Request(*this,"setField")
			.push(str.c_str())
			.call()
			.pop(rc);

		if(rc < 0)
			chkResponse(-rc);

	}

	void IPC::Session::set(int baddr, const std::string &str) {

		int32_t rc;

		Request(*this,"setStringAtAddress")
			.push((int32_t) baddr)
			.push(str.c_str())
			.call()
			.pop(rc);

		if(rc < 0)
			chkResponse(-rc);

	}

	void IPC::Session::set(int row, int col, const std::string &str) {

		int32_t rc;

		Request(*this,"setStringAt")
			.push((uint32_t) row)
			.push((uint32_t) col)
			.push(str.c_str())
			.call()
			.pop(rc);

		if(rc < 0)
			chkResponse(-rc);

	}

 }


