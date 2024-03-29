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
 * @brief Implements WIN32 request "push" methods.
 *
 * @author perry.werneck@gmail.com
 *
 */

 #ifdef HAVE_CONFIG_H
	#include <config.h>
 #endif // HAVE_CONFIG_H

 #include <ipc-client-internals.h>
 #include "pipe-request.h"

 using namespace std;

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	TN3270::Request & Pipe::Request::push(const char *arg) {
		(*this->outvalues)++;
		debug("Argument ",(*this->outvalues)," is '",arg,"' with type '",((char) Pipe::Request::String),"'");
		pushBlock(arg, strlen(arg)+1)->type = Pipe::Request::String;
		return *this;
	}

	TN3270::Request & Pipe::Request::push(const bool arg) {
		(*this->outvalues)++;
		uint8_t value = (uint8_t) (arg ? 0xff : 0);
		pushBlock(&value, sizeof(value))->type = Pipe::Request::Boolean;
		return *this;
	}

	TN3270::Request & Pipe::Request::push(const uint8_t arg) {
		(*this->outvalues)++;
		pushBlock(&arg, sizeof(arg))->type = Pipe::Request::Uchar;
		return *this;
	}

	TN3270::Request & Pipe::Request::push(const int32_t arg) {
		(*this->outvalues)++;
		pushBlock(&arg, sizeof(arg))->type = Pipe::Request::Int32;
		return *this;
	}

	TN3270::Request & Pipe::Request::push(const uint32_t arg) {
		(*this->outvalues)++;
		pushBlock(&arg, sizeof(arg))->type = Pipe::Request::Uint32;
		return *this;
	}

 }


