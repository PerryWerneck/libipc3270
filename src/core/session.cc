/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) <2008> <Banco do Brasil S.A.>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * @file src/core/session.cc
 *
 * @brief Implements common session object.
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include <config.h>
 #include <ipc-client-internals.h>
 #include <lib3270/ipc/session.h>
 #include <private/session.h>
 #include <string>
 #include <cstring>
 #include <stdexcept>

 using namespace std;

 namespace TN3270 {

	void Session::connect(time_t seconds) {
		connect(nullptr,seconds);
	}

	std::shared_ptr<Session> Session::getInstance(const char *id, const char *charset) {

		if(id && *id) {
			return Abstract::Session::getRemoteInstance(id,charset);
		}

		return Abstract::Session::getLocalInstance(charset);
	}

	Session::Session() {
	}

	Session::~Session() {
	}

	std::shared_ptr<Request> Session::RequestFactory(const Request::Type, const char *) const {
		throw runtime_error("The back end is unable to handle remote requests");
	}


	/// @brief Fire event.
	void Session::fire(const Event &) {
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

				// Insert text.
				push(text,sz);
			}

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

			if(rc)
				return rc;

			text = (ptr+1);
		}

		sz = strlen(text);
		if(sz) {
			push(text,sz);
		}

		return rc;
	}

	bool Session::contains(const char *str) {
		return toString((int) 0,(int) -1, '\0').find(str) != string::npos;
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

	void Session::setProperty(const char *name, const int value) {
		debug("Property ",name,"=",value);
		RequestFactory(Request::SetProperty,name)->push(value).call();
	}

	void Session::setProperty(const char *name, const unsigned int value) {
		debug("Property ",name,"=",value);
		RequestFactory(Request::SetProperty,name)->push(value).call();
	}

	void Session::setProperty(const char *name, const bool value) {
		debug("Property ",name,"=",value);
		RequestFactory(Request::SetProperty,name)->push(value).call();
	}

	void Session::setProperty(const char *name, const char *value) {
		debug("Property ",name,"=",value);
		RequestFactory(Request::SetProperty,name)->push(value).call();
	}

 }



