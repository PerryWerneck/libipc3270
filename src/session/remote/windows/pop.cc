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
 * @file src/os/windows/request.cc
 *
 * @brief Implements WIN32 request methods.
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

	class InvalidFormatException : public std::exception {
	private:
		string message;

	public:
		InvalidFormatException(const Pipe::Request::Type received, const Pipe::Request::Type expected) {

			message = "Invalid format on IPC package, expecting \'";
			message += (char) expected;
			message += "\' but received \'";
			message += (char) received;
			message += "\'";

		}

		const char * what() const noexcept override {
			return message.c_str();
		}
	};

	TN3270::Request & Pipe::Request::pop(std::string &value) {

		DataBlock * block = getNextBlock();

		if(block->type != Pipe::Request::String)
			throw InvalidFormatException(block->type, Pipe::Request::String);

		const char *ptr = (const char *) (block+1);

		in.current += (strlen(ptr)+1+sizeof(DataBlock));

		value.assign(ptr);

		return *this;
	}

	TN3270::Request & Pipe::Request::Request::pop(int &value) {

		DataBlock * block = getNextBlock();

		debug("Popping 'int' value");

		switch(block->type) {
		case Pipe::Request::Uint16:
			value = * ((uint16_t *) (block+1));
			in.current += sizeof(uint16_t) + sizeof(DataBlock);
			break;

		case Pipe::Request::Int16:
			value = * ((int16_t *) (block+1));
			in.current += sizeof(int16_t) + sizeof(DataBlock);
			break;

		case Pipe::Request::Int32:
			value = * ((int32_t *) (block+1));
			in.current += sizeof(int32_t) + sizeof(DataBlock);
			break;

		case Pipe::Request::Int64:
			value = * ((int64_t *) (block+1));
			in.current += sizeof(int64_t) + sizeof(DataBlock);
			break;

		default:
			throw InvalidFormatException(block->type, Pipe::Request::Int16);
		}

		return *this;
	}

	TN3270::Request & Pipe::Request::Request::pop(unsigned int &value) {

		debug("Popping 'unsigned int' value");

		DataBlock * block = getNextBlock();

		switch(block->type) {
		case Pipe::Request::Int16:
			value = * ((int16_t *) (block+1));
			in.current += sizeof(int16_t) + sizeof(DataBlock);
			break;

		case Pipe::Request::Uint16:
			value = * ((uint16_t *) (block+1));
			in.current += sizeof(uint16_t) + sizeof(DataBlock);
			break;

		case Pipe::Request::Uint32:
			value = * ((uint32_t *) (block+1));
			in.current += sizeof(uint32_t) + sizeof(DataBlock);
			break;

		case Pipe::Request::Uint64:
			value = * ((uint64_t *) (block+1));
			in.current += sizeof(uint64_t) + sizeof(DataBlock);
			break;

		default:
			throw InvalidFormatException(block->type, Pipe::Request::Uint16);
		}

		return *this;
	}

	TN3270::Request & Pipe::Request::Request::pop(bool &value) {

		DataBlock * block = getNextBlock();

		switch(block->type) {
		case Pipe::Request::Boolean:
			value = (* ((uint8_t *) (block+1))) != 0;
			in.current += sizeof(uint8_t) + sizeof(DataBlock);
			break;

		case Pipe::Request::Uint16:
			value = (* ((uint16_t *) (block+1))) != 0;
			in.current += sizeof(uint16_t) + sizeof(DataBlock);
			break;

		case Pipe::Request::Uint32:
			value = (* ((uint32_t *) (block+1))) != 0;
			in.current += sizeof(uint32_t) + sizeof(DataBlock);
			break;

		case Pipe::Request::Uint64:
			value = (* ((uint64_t *) (block+1))) != 0;
			in.current += sizeof(uint64_t) + sizeof(DataBlock);
			break;

		default:
			throw InvalidFormatException(block->type, Pipe::Request::Boolean);
		}

		return *this;
	}

 }


