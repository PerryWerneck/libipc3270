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
 * @file src/core/host.cc
 *
 * @brief Implements host object.
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include <ipc-client-internals.h>
 #include <string>
 #include <cstring>
 #include <iostream>

 using std::string;
 using std::clog;
 using std::endl;

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	Host::Host(const char *id, const char *url, time_t timeout) {

		debug("Creating host id=\"", id);

		this->timeout = timeout;
		this->session = Session::getInstance(id);
		if(url) {
			this->connect(url);
		}
	}

	Host::~Host() {
		delete this->session;
		this->session = nullptr;
	}

    Host & Host::connect(const char *url, bool sync) {
        this->session->connect(url);
        if(sync) {
			this->sync();
        }
        return *this;
    }

	Host & Host::disconnect() {
        this->session->disconnect();
        return *this;
	}

	Host & Host::waitForReady(time_t timeout) {
		this->session->waitForReady(timeout);
		return *this;
	}

	/// @brief Writes characters to the associated file from the put area
	int Host::sync() {
        this->session->waitForReady(this->timeout);
		return 0;
	}

	/// @brief Writes characters to the associated output sequence from the put area.
	int Host::overflow(int c) {

		if (c != EOF) {
			char str[] = { static_cast<char>(c), 0 };
			this->session->push((const char *) str);
		} else {
			sync();
		}

		return c;

	}

	bool Host::isReady() const {
		return getProgramMessage() == MESSAGE_NONE;
	}

	bool Host::isConnected() const {
		return getConnectionState() == CONNECTED_TN3270E;
	}

	std::string Host::toString() const {

        this->session->waitForReady(this->timeout);

	    if(this->session->getConnectionState() == TN3270::DISCONNECTED) {
            throw std::system_error(ENOTCONN, std::system_category());
	    }

	    return this->session->toString();
	}

	std::string Host::toString(int baddr, size_t len, char lf) const {

        this->session->waitForReady(this->timeout);

	    if(this->session->getConnectionState() == TN3270::DISCONNECTED) {
            throw std::system_error(ENOTCONN, std::system_category());
	    }

	    return this->session->toString(baddr,len,lf);

	}

	std::string Host::toString(int row, int col, size_t sz, char lf) const {

        this->session->waitForReady(this->timeout);

	    if(this->session->getConnectionState() == TN3270::DISCONNECTED) {
            throw std::system_error(ENOTCONN, std::system_category());
	    }

	    return this->session->toString(row,col,sz,lf);


	}

	Host & Host::pop(int baddr, std::string &text) {

        this->session->waitForReady(this->timeout);

	    if(this->session->getConnectionState() == TN3270::DISCONNECTED) {
            throw std::system_error(ENOTCONN, std::system_category());
	    }

		session->pop(baddr, text);

		return *this;
	}

	Host & Host::pop(int row, int col, std::string &text) {

        this->session->waitForReady(this->timeout);

	    if(this->session->getConnectionState() == TN3270::DISCONNECTED) {
            throw std::system_error(ENOTCONN, std::system_category());
	    }

		session->pop(row,col,text);

		return *this;
	}

	Host & Host::pop(std::string &text) {

        this->session->waitForReady(this->timeout);

	    if(this->session->getConnectionState() == TN3270::DISCONNECTED) {
            throw std::system_error(ENOTCONN, std::system_category());
	    }

		session->pop(text);

		return *this;
	}

	/*
	Host & Host::input(const char *text, size_t sz) {
		this->session->input(text, sz);
		return *this;
	}


	Host & Host::input(const char *text, const char control_char) {

		for(const char * ptr = strchr(text,control_char); ptr; ptr = strchr(text,control_char)) {

			input(text,(size_t) (ptr-text) );

			switch(*(++ptr)) {
			case 'P':	// Print
				print();
				break;

			case 'E':	// Enter
				push(ENTER);
				break;

			case 'F':	// Erase EOF
				push(ERASE_EOF);
				break;

			case '1':	// PF1
				push(PF_1);
				break;

			case '2':	// PF2
				push(PF_2);
				break;

			case '3':	// PF3
				push(PF_3);
				break;

			case '4':	// PF4
				push(PF_4);
				break;

			case '5':	// PF5
				push(PF_5);
				break;

			case '6':	// PF6
				push(PF_6);
				break;

			case '7':	// PF7
				push(PF_7);
				break;

			case '8':	// PF8
				push(PF_8);
				break;

			case '9':	// PF9
				push(PF_9);
				break;

			case 'a':	// PF10
				push(PF_10);
				break;

			case 'b':	// PF11
				push(PF_11);
				break;

			case 'c':	// PF12
				push(PF_12);
				break;

			case 'd':	// PF13
				push(PF_13);
				break;

			case 'e':	// PF14
				push(PF_14);
				break;

			case 'f':	// PF15
				push(PF_15);
				break;

			case 'g':	// PF16
				push(PF_16);
				break;

			case 'h':	// PF17
				push(PF_17);
				break;

			case 'i':	// PF18
				push(PF_18);
				break;

			case 'j':	// PF19
				push(PF_19);
				break;

			case 'k':	// PF20
				push(PF_20);
				break;

			case 'l':	// PF21
				push(PF_21);
				break;

			case 'm':	// PF22
				push(PF_22);
				break;

			case 'n':	// PF23
				push(PF_23);
				break;

			case 'o':	// PF24
				push(PF_24);
				break;

			case '@':	// Send '@' character
				input("@",(size_t) 1);
				break;

			case 'x':	// PA1
				push(PA_1);
				break;

			case 'y':	// PA2
				push(PA_2);
				break;

			case 'z':	// PA3
				push(PA_3);
				break;

			case 'B':	// PC_LEFTTAB = "@B"
				break;

			case 'T':	// PC_RIGHTTAB = "@T"
				break;

			case 'N':	// PC_NEWLINE = "@N"
				push(NEWLINE);
				break;

			case 'C':	// PC_CLEAR = "@C"
				push(CLEAR);
				break;

			case 'D':	// PC_DELETE = "@D"
				push(CHAR_DELETE);
				break;

			case 'H':	// PC_HELP = "@H"
				break;

			case 'I':	// PC_INSERT = "@I"
				break;

			case 'L':	// PC_CURSORLEFT = "@L"
				break;

			case 'R':	// PC_RESET = "@R"
				push(KYBD_RESET);
				break;

			case 'U':	// PC_CURSORUP = "@U"
				break;

			case 'V':	// PC_CURSORDOWN = "@V"
				break;

			case 'Z':	// PC_CURSORRIGHT = "@Z"
				break;

			case '0':	// PC_HOME = "@0"
				break;

			case 'p':	// PC_PLUSKEY = "@p"
				break;

			case 'q':	// PC_END = "@q"
				break;

			case 's':	// PC_SCRLK = "@s"
				break;

			case 't':	// PC_NUMLOCK = "@t"
				break;

			case 'u':	// PC_PAGEUP = "@u"
				break;

			case 'v':	// PC_PAGEDOWN = "@v"
				break;

			case '/':	// PC_OVERRUNOFQUEUE = "@/"   ' Queue overflow, used in Get Key only
				break;

			case '$':	// PC_ALTCURSOR = "@$"        ' Presentation Manager only, unused in VB environment
				break;

			case '<':	// PC_BACKSPACE = "@<"
				push(BACKSPACE);
				break;


			// Global Const PC_TEST = "@A@C"
			// Global Const PC_WORDDELETE = "@A@D"
			// Global Const PC_FIELDEXIT = "@A@E"
			// Global Const PC_ERASEINPUT = "@A@F"
			// Global Const PC_SYSTEMREQUEST = "@A@H"
			// Global Const PC_INSERTTOGGLE = "@A@I"
			// Global Const PC_CURSORSELECT = "@A@J"
			// Global Const PC_CURSLEFTFAST = "@A@L"
			// Global Const PC_GETCURSOR = "@A@N"
			// Global Const PC_LOCATECURSOR = "@A@O"
			// Global Const PC_ATTENTION = "@A@Q"
			// Global Const PC_DEVICECANCEL = "@A@R"
			// Global Const PC_PRINTPS = "@A@T"
			// Global Const PC_CURSUPFAST = "@A@U"
			// Global Const PC_CURSDOWNFAST = "@A@V"
			// Global Const PC_HEX = "@A@X"
			// Global Const PC_FUNCTIONKEY = "@A@Y"
			// Global Const PC_CURSRIGHTFAST = "@A@Z"

			// Global Const PC_REVERSEVIDEO = "@A@9"
			// Global Const PC_UNDERSCORE = "@A@b"
			// Global Const PC_BLINK = "@A@c"
			// Global Const PC_RED = "@A@d"BACKSPACE
			// Global Const PC_PINK = "@A@e"
			// Global Const PC_GREEN = "@A@f"
			// Global Const PC_YELLOW = "@A@g"
			// Global Const PC_BLUE = "@A@h"
			// Global Const PC_TURQOISE = "@A@i"
			// Global Const PC_WHITE = "@A@j"
			// Global Const PC_RSTHOSTCOLORS = "@A@l"
			// Global Const PC_PRINTPC = "@A@t"

			// Global Const PC_FIELDMINUS = "@A@-"
			// Global Const PC_FIELDPLUS = "@A@+"
			}

			text = (ptr+1);
		}

		if(*text) {
			input(text,strlen(text));
		}

		return *this;

	}
	*/

 }

