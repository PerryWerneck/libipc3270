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
 #include <lib3270/trace.h>

 using std::string;

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	#define PIPE_BUFFER_LENGTH 8192

#ifdef DEBUG

	// From lib3270_trace_data
	static void trace_data(const char *msg, const unsigned char *data, size_t datalen)
{
	// 00000000001111111111222222222233333333334444444444555555555566666666667777777777
	// 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	// xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx . . . . . . . . . . . . . . . .

	size_t ix;
	char buffer[80];
	char hexvalue[3];

	memset(buffer,0,sizeof(buffer));

	std::cout << msg << "(" << datalen << " bytes)" << std::endl;

	for(ix = 0; ix < datalen; ix++)
	{
		size_t col = (ix%15);

		if(col == 0)
		{
			if(ix) {
				std::cout << "\t" << buffer << std::endl;
			}

			memset(buffer,' ',79);
			buffer[79] = 0;
		}

		snprintf(hexvalue,3,"%02x",data[ix]);
		memcpy(buffer+(col*3),hexvalue,2);

		if(data[ix] > ' ')
			buffer[48 + (col*2)] = data[ix];

	}

	std::cout << "\t" << buffer << std::endl;

}

#endif // DEBUG


	/*
	IPC::Request::Request(HANDLE hPipe, const char *name, uint16_t type) {

		this->hPipe = hPipe;

		// Create buffers
		in.length = PIPE_BUFFER_LENGTH;
		in.used = 0;
		in.block = new uint8_t[in.length];

		out.length = PIPE_BUFFER_LENGTH;
		out.used = 0;
		out.block = new uint8_t[out.length];

		// Add name
		strcpy((char *) out.block, name);
		out.used += strlen((char *) name) + 1;

		// Add type
		debug("Request type stored @",out.used);

		*((uint16_t *) (out.block + out.used)) = type;
		out.used += sizeof(uint16_t);

		// Add argument counter.
		this->outvalues = (uint16_t *) (out.block + out.used);
		out.used += sizeof(uint16_t);

		*this->outvalues = 0;

	}

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

	IPC::Request & IPC::Request::call() {

#ifdef DEBUG
		// trace_data("Request block",(const unsigned char *) this->out.block, this->out.used);
#endif // DEBUG

		debug("Sending request with ", *this->outvalues, " elements");

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

		//
		// Heaer format:
		//
		// STRING	Response name
		// uint16_t	Return code
		// uint16_t	Arguments
		//
		// Data block:
		//
		//

#ifdef DEBUG
		trace_data("Response block",(const unsigned char *) this->in.block, this->in.used);
#endif // DEBUG

		// Extract response name
		in.current = strlen((const char *) in.block)+1;

		// Extract return code
		uint16_t rc = *((uint16_t *) (in.block + in.current));
		in.current += sizeof(uint16_t);

		if(rc) {

			// It´s an error, extract message
			in.block[in.used] = 0;
			debug("Error was ",rc," (\"",(const char *) (in.block + in.current),"\")");

			// Overload system error mostly because of the lack of ENOTCONN message on windows.
			class Error : public std::system_error {
			private:
				std::string message;

			public:
				Error(int rc, const char *msg) : std::system_error((int) rc, std::generic_category()), message(msg) {
				}

				const char *what() const noexcept override {
					return message.c_str();
				}
			};

 			throw Error(rc, (const char *) (in.block + in.current));

		}

		// It´s not an error, extract argument count
		uint16_t argc = *((uint16_t *) (in.block + in.current));
		in.current += sizeof(uint16_t);

		debug("Received response \"", ((const char *) in.block), "\" with rc=", rc, " and ", argc, " arguments");

		return *this;
	}
	*/

 }


