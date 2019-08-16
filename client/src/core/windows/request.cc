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

	IPC::Request::~Request() {

		delete[] ((uint8_t *) in.block);
		delete[] ((uint8_t *) out.block);

	}

	/// @brief Store value on data block.
	IPC::Request::DataBlock * IPC::Request::pushBlock(const void *ptr, size_t length) {

		if((out.used + length + sizeof(IPC::Request::DataBlock)) >= out.length) {
			throw std::runtime_error("Too big");
		}

		IPC::Request::DataBlock * rc = (IPC::Request::DataBlock *) (out.block + out.used);
		memcpy(((uint8_t *) (rc+1)), ((uint8_t *) ptr), length);

		out.used += (sizeof(IPC::Request::DataBlock) + length);

		return rc;

	}

	/// @brief Get next argument.
	IPC::Request::DataBlock * IPC::Request::getNextBlock() const {

		if((in.current + sizeof(IPC::Request::DataBlock)) >= in.used) {
			throw std::runtime_error("Out of range");
		}

		return (IPC::Request::DataBlock *) (in.block + in.current);

	}

	IPC::Request & IPC::Request::push(const char *arg) {
		pushBlock(arg, strlen(arg)+1)->type = IPC::Request::String;
		return *this;
	}

	IPC::Request & IPC::Request::push(const bool arg) {
		uint8_t value = (uint8_t) (arg ? 0xff : 0);
		pushBlock(&value, sizeof(value))->type = IPC::Request::Boolean;
		return *this;
	}

	IPC::Request & IPC::Request::push(const uint8_t arg) {
		pushBlock(&arg, sizeof(arg))->type = IPC::Request::Uchar;
		return *this;
	}

	IPC::Request & IPC::Request::push(const int32_t arg) {
		pushBlock(&arg, sizeof(arg))->type = IPC::Request::Int32;
		return *this;
	}

	IPC::Request & IPC::Request::push(const uint32_t arg) {
		pushBlock(&arg, sizeof(arg))->type = IPC::Request::Uint32;
		return *this;
	}

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

	IPC::Request & IPC::Request::call() {

#ifdef DEBUG
		// lib3270_trace_data(NULL,"Request block",(const char *) this->out.block, this->out.used);
#endif // DEBUG

		in.current = 0;

		if(!TransactNamedPipe(
				this->hPipe,
				this->out.block,
				this->out.used,
				this->in.block,
				this->in.length,
				&this->in.used,
				NULL)
			) {

			throw std::runtime_error("Can't transact on IPC Channel");

		}

		debug("Received response \"", in.block, "\" with ", in.used, " bytes");
#ifdef DEBUG
		// lib3270_trace_data(NULL,"Response block",(const char *) this->in.block, this->in.used);
#endif // DEBUG

		// Extract response name
		in.current = strlen((const char *) in.block)+1;

		// Extract return code
		uint16_t rc = *((uint16_t *) (in.block + in.current));
		in.current += sizeof(uint16_t);

		// Extract argc
		uint16_t argc = *((uint16_t *) (in.block + in.current));
		in.current += sizeof(uint16_t);

		debug("Received response \"", ((const char *) in.block), "\" with rc=", rc, " and ", argc, " arguments");

		return *this;
	}

 }


