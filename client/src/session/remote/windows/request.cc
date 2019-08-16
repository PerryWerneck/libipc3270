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
 * @brief Implements WIN32 request constructors based on TN3270::IPC::Session.
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include "../private.h"

 using std::string;

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	#define PIPE_BUFFER_LENGTH 8192

	IPC::Request::Request(const Session &session) {

		this->hPipe = session.hPipe;

		in.length = PIPE_BUFFER_LENGTH;
		in.used = 0;
		in.block = new uint8_t[in.length];

		out.length = PIPE_BUFFER_LENGTH;
		out.used = 0;
		out.block = new uint8_t[out.length];

	}

	IPC::Request::Request(const Session &session, const char *method) : Request(session) {

		// Add name
		strcpy((char *) out.block, method);
		out.used += strlen((char *) method) + 1;

		// Add ID
		*((uint16_t *) (out.block + out.used)) = (uint16_t) 3;
		out.used += sizeof(uint16_t);

	}

	IPC::Request::Request(const Session &session, bool isSet, const char *property) : Request(session) {

		// Add name
		strcpy((char *) out.block, property);
		out.used += strlen((char *) property) + 1;

		// Add ID (SetProperty = 2, getProperty = 1)
		*((uint16_t *) (out.block + out.used)) = (uint16_t) (isSet ? 2 : 1);
		out.used += sizeof(uint16_t);

	}

 }

