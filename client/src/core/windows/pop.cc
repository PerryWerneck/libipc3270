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

 #include <ipc-client-internals.h>

 using std::string;

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	IPC::Request & IPC::Request::pop(std::string &value) {
		DataBlock * block = getNextBlock();

		if(block->type != IPC::Request::String)
			throw std::runtime_error("Invalid format");

		const char *ptr = (const char *) (block+1);

		in.current += (strlen(ptr)+1+sizeof(DataBlock));

		value.assign(ptr);

		return *this;
	}

	IPC::Request & IPC::Request::Request::pop(int &value) {

		DataBlock * block = getNextBlock();

		switch(block->type) {
		case IPC::Request::Int16:
			value = * ((int16_t *) (block+1));
			in.current += sizeof(int16_t) + sizeof(DataBlock);
			break;

		case IPC::Request::Int32:
			value = * ((int32_t *) (block+1));
			in.current += sizeof(int32_t) + sizeof(DataBlock);
			break;

		case IPC::Request::Int64:
			value = * ((int64_t *) (block+1));
			in.current += sizeof(int64_t) + sizeof(DataBlock);
			break;

		default:
			throw std::runtime_error("Invalid format");
		}

		return *this;
	}

	IPC::Request & IPC::Request::Request::pop(unsigned int &value) {

		DataBlock * block = getNextBlock();

		switch(block->type) {
		case IPC::Request::Uint16:
			value = * ((uint16_t *) (block+1));
			in.current += sizeof(uint16_t) + sizeof(DataBlock);
			break;

		case IPC::Request::Uint32:
			value = * ((uint32_t *) (block+1));
			in.current += sizeof(uint32_t) + sizeof(DataBlock);
			break;

		case IPC::Request::Uint64:
			value = * ((uint64_t *) (block+1));
			in.current += sizeof(uint64_t) + sizeof(DataBlock);
			break;

		default:
			throw std::runtime_error("Invalid format");
		}

		return *this;
	}

	IPC::Request & IPC::Request::Request::pop(bool &value) {

		DataBlock * block = getNextBlock();

		switch(block->type) {
		case IPC::Request::Boolean:
			value = (* ((uint8_t *) (block+1))) != 0;
			in.current += sizeof(uint8_t) + sizeof(DataBlock);
			break;

		case IPC::Request::Uint16:
			value = (* ((uint16_t *) (block+1))) != 0;
			in.current += sizeof(uint16_t) + sizeof(DataBlock);
			break;

		case IPC::Request::Uint32:
			value = (* ((uint32_t *) (block+1))) != 0;
			in.current += sizeof(uint32_t) + sizeof(DataBlock);
			break;

		case IPC::Request::Uint64:
			value = (* ((uint64_t *) (block+1))) != 0;
			in.current += sizeof(uint64_t) + sizeof(DataBlock);
			break;

		default:
			throw std::runtime_error("Invalid format");
		}

		return *this;
	}

 }


