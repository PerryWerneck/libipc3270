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
 * @file src/core/session.cc
 *
 * @brief Implements common session object.
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include <ipc-client-internals.h>
 #include <string>
 #include <cstring>

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	Session * Session::getInstance(const char *id, const char *charset) {

		if(!(id && *id)) {
			return Local::getSessionInstance(charset);
		}

		if(*id == ':') {

			std::string name{LIB3270_STRINGIZE_VALUE_OF(PRODUCT_NAME)};
			name += id;

			return IPC::getSessionInstance(name.c_str(), charset);

		}

		return IPC::getSessionInstance(id, charset);

	}

	Session::Session() {
	}

	Session::~Session() {
	}

	/// @brief Fire event.
	void Session::fire(const Event GNUC_UNUSED(&event)) {
	}

	void Session::push(const PFKey key) {
		pfkey( ((unsigned short) key) + 1);
	}

	void Session::push(const PAKey key) {
		pakey( ((unsigned short) key) + 1);
	}

	LIB3270_KEYBOARD_LOCK_STATE Session::input(const std::string &str, const char control_char) {

		LIB3270_KEYBOARD_LOCK_STATE rc = (LIB3270_KEYBOARD_LOCK_STATE) 0;

		size_t sz;
		const char * text = str.c_str();

		for(const char * ptr = strchr(text,control_char); ptr; ptr = strchr(text,control_char)) {

			sz = (size_t) (ptr-text);
			if(sz) {

				// Wait for unlock, insert text.
				if( (rc = waitForKeyboardUnlock()) != 0) {
					return rc;
				}
				push(text,sz);
			}

			switch(*(++ptr)) {
			case 'P':	// Print
				print();
				break;

			case 'E':	// Enter
				push(ENTER);
				rc = waitForKeyboardUnlock();
				break;

			case 'F':	// Erase EOF
				push(ERASE_EOF);
				break;

			case '1':	// PF1
				push(PF_1);
				rc = waitForKeyboardUnlock();
				break;

			case '2':	// PF2
				push(PF_2);
				rc = waitForKeyboardUnlock();
				break;

			case '3':	// PF3
				push(PF_3);
				rc = waitForKeyboardUnlock();
				break;

			case '4':	// PF4
				push(PF_4);
				rc = waitForKeyboardUnlock();
				break;

			case '5':	// PF5
				push(PF_5);
				rc = waitForKeyboardUnlock();
				break;

			case '6':	// PF6
				push(PF_6);
				rc = waitForKeyboardUnlock();
				break;

			case '7':	// PF7
				push(PF_7);
				rc = waitForKeyboardUnlock();
				break;

			case '8':	// PF8
				push(PF_8);
				rc = waitForKeyboardUnlock();
				break;

			case '9':	// PF9
				push(PF_9);
				rc = waitForKeyboardUnlock();
				break;

			case 'a':	// PF10
				push(PF_10);
				rc = waitForKeyboardUnlock();
				break;

			case 'b':	// PF11
				push(PF_11);
				rc = waitForKeyboardUnlock();
				break;

			case 'c':	// PF12
				push(PF_12);
				rc = waitForKeyboardUnlock();
				break;

			case 'd':	// PF13
				push(PF_13);
				rc = waitForKeyboardUnlock();
				break;

			case 'e':	// PF14
				push(PF_14);
				rc = waitForKeyboardUnlock();
				break;

			case 'f':	// PF15
				push(PF_15);
				rc = waitForKeyboardUnlock();
				break;

			case 'g':	// PF16
				push(PF_16);
				rc = waitForKeyboardUnlock();
				break;

			case 'h':	// PF17
				push(PF_17);
				rc = waitForKeyboardUnlock();
				break;

			case 'i':	// PF18
				push(PF_18);
				rc = waitForKeyboardUnlock();
				break;

			case 'j':	// PF19
				push(PF_19);
				rc = waitForKeyboardUnlock();
				break;

			case 'k':	// PF20
				push(PF_20);
				rc = waitForKeyboardUnlock();
				break;

			case 'l':	// PF21
				push(PF_21);
				rc = waitForKeyboardUnlock();
				break;

			case 'm':	// PF22
				push(PF_22);
				rc = waitForKeyboardUnlock();
				break;

			case 'n':	// PF23
				push(PF_23);
				rc = waitForKeyboardUnlock();
				break;

			case 'o':	// PF24
				push(PF_24);
				rc = waitForKeyboardUnlock();
				break;

			case '@':	// Send '@' character
				input("@",(size_t) 1);
				break;

			case 'x':	// PA1
				push(PA_1);
				rc = waitForKeyboardUnlock();
				break;

			case 'y':	// PA2
				push(PA_2);
				rc = waitForKeyboardUnlock();
				break;

			case 'z':	// PA3
				push(PA_3);
				rc = waitForKeyboardUnlock();
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

			if(rc)
				return rc;

			text = (ptr+1);
		}

		sz = strlen(text);
		if(sz) {
			if( (rc = waitForKeyboardUnlock()) != 0) {
				return rc;
			}
			push(text,sz);
		}

		return rc;
	}

	/// @brief Checks if the terminal contains the string.
	size_t Session::find(const char * str, size_t pos) const {
		return toString((int) 0,(int) -1, '\0').find(str,pos);
	}

	/// @brief Get the number of occurrences of a string in the terminal.
	size_t Session::count(const char * str, size_t pos) const {

		std::string contents = toString((int) 0,(int) -1, '\0');
		size_t rc = 0;

		while( (pos = contents.find(str,pos)) != std::string::npos) {
			rc++;
		}

		return rc;

	}

	/// @brief Compare contents.
	int Session::compare(int baddr, const char* s, int len) const {

		if(len < 0)
			len = strlen(s);

		return toString(baddr,len,'\0').compare(0,len,s);
	}

	int Session::compare(unsigned short row, unsigned short col, const char* s, int len) const {

		if(len < 0)
			len = strlen(s);

		return toString(row, col, len, '\0').compare(0,len,s);

	}

	void Session::setAttribute(const char GNUC_UNUSED(*name), const int GNUC_UNUSED(value)) {
		throw std::system_error(ENOTSUP, std::system_category());
	}

	void Session::setAttribute(const char GNUC_UNUSED(*name), const char GNUC_UNUSED(*value)) {
		throw std::system_error(ENOTSUP, std::system_category());
	}

	std::vector<Attribute> Session::getAttributes() const {
		std::vector<Attribute> attributes;
		this->getAttributes(attributes);
		return attributes;
	}

	void Session::getAttribute(const char *name, int &value) const {
		value = getAttribute(name).getInt32();
	}

	void Session::getAttribute(const char *name, unsigned int &value) const {
		value = getAttribute(name).getUint32();
	}

	void Session::getAttribute(const char *name, std::string &value) const {
		value.assign(value = getAttribute(name).getString().c_str());
	}

	void Session::getAttribute(const char *name, bool &value) const {
		value = getAttribute(name).getBoolean();
	}

	/// @brief Create an action object
	Action * Session::getAction(const LIB3270_ACTION GNUC_UNUSED(*descriptor)) {
		throw std::system_error(ENOTSUP, std::system_category());
	}

	Action * Session::getAction(const char *name) {

		for(auto action : getActions()) {

			if(strcasecmp(action->name,name) == 0) {
				return getAction(action);
			}

		}

		throw std::system_error(EINVAL, std::system_category());

	}


 }



