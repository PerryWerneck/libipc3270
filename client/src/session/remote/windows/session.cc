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
 * @file src/os/windows/session.cc
 *
 * @brief Implements WIN32 remote session methods.
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include "../private.h"
 #include <ipc-client-internals.h>
 #include <cstring>
 #include <lib3270/trace.h>
 #include <algorithm>

 using std::string;

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	IPC::Session::Session(const char *id, const char *charset) : Abstract::Session() {

		const char *ptr = strchr(id,':');

		if(!ptr)
			throw std::system_error(EINVAL, std::system_category());

		string pipename{"\\\\.\\pipe\\"};

		pipename += string(id,ptr - id);
		pipename += "\\";
		pipename += (ptr+1);

		std::transform(pipename.begin(), pipename.end(), pipename.begin(), ::tolower);

		debug("id: \"", id, "\" pipename: \"", pipename , "\"");

		this->hPipe = CreateFile(
			TEXT(pipename.c_str()),		// pipe name
			GENERIC_READ |  			// read and write access
			GENERIC_WRITE,
			0,              			// no sharing
			NULL,           			// default security attributes
			OPEN_EXISTING,  			// opens existing pipe
			0,              			// default attributes
			NULL						// no template file
		);

		if (hPipe == INVALID_HANDLE_VALUE) {
			throw std::runtime_error("Can't open IPC Channel");
		}

		// The pipe connected; change to message-read mode.
		DWORD dwMode = PIPE_READMODE_MESSAGE;
		if(!SetNamedPipeHandleState(hPipe,&dwMode,NULL,NULL)) {
			throw std::runtime_error("Can't set IPC Channel mode");
		}

		this->setCharSet(charset);
	}

	IPC::Session::~Session() {
		CloseHandle(this->hPipe);
	}

 }


