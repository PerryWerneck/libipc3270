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
 * @file src/session/remote/session.cc
 *
 * @brief Implements lib3270 access using IPC calls.
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include <ipc-client-internals.h>
 #include <cstring>

#ifndef _WIN32
	#include <unistd.h>	// sleep
#endif // _WIN32

 using std::string;

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	void IPC::Session::connect(const char *url) {
		Request request(*this,"connect");
		request.push(url).call();
    }

	void IPC::Session::disconnect() {
		Request(*this,"disconnect").call();
	}

	// Wait for session state.
	void IPC::Session::waitForReady(time_t timeout) {

		int rc;

		time_t end = time(nullptr) + timeout;

		while(time(nullptr) < end) {

			debug("Running waitForReady request...");

			Request(*this,"waitForReady")
				.push((uint32_t) 1)
				.call()
				.pop(rc);

			debug("Wait for ready returned ",rc);

			if(rc == 0)
				return;

		}

		throw std::system_error(ETIMEDOUT, std::system_category());

	}

	std::string	IPC::Session::toString(int baddr, size_t len, char lf) const {

		std::string rc;

		Request(*this,"getStringAtAddress")
			.push((uint32_t) baddr)
			.push((uint32_t) len)
			.push((uint8_t) lf)
			.call()
			.pop(rc);

		return rc;
	}

	std::string	IPC::Session::toString(int row, int col, size_t sz, char lf) const {

		std::string rc;

		Request(*this,"getStringAt")
			.push((uint32_t) row)
			.push((uint32_t) col)
			.push((uint32_t) sz)
			.push((uint8_t) lf)
			.call()
			.pop(rc);

		return rc;
	}

	ProgramMessage IPC::Session::getProgramMessage() const {

		int program_message;
		getProperty("program_message",program_message);
		return (ProgramMessage) program_message;

	}

	ConnectionState IPC::Session::getConnectionState() const {

		int cstate;
		getProperty("cstate",cstate);
		return (ConnectionState) cstate;

	}

	TN3270::Session & IPC::Session::input(const char *text, size_t length) {

		throw std::system_error(ENOTSUP, std::system_category());

		return *this;
	}

	/// @brief Set field at current position, jumps to next writable field.
	TN3270::Session & IPC::Session::push(const char *text) {

		int rc;

		Request(*this,"setString")
			.push(text)
			.call()
			.pop(rc);

		if(rc) {
            throw std::system_error((int) rc, std::system_category());
		}

		return *this;

	}

	TN3270::Session & IPC::Session::push(int baddr, const char *text, int length) {

		int rc;

		if(length < 0)
			length = strlen(text);

		Request(*this,"setStringAtAddress")
			.push((uint32_t) baddr)
			.push(text)
			.push((uint32_t) length)
			.call()
			.pop(rc);

		if(rc) {
            throw std::system_error((int) rc, std::system_category());
		}

		return *this;

	}

	TN3270::Session & IPC::Session::push(int row, int col, const char *text, int length) {

		int32_t rc;

		if(length < 0)
			length = strlen(text);

		Request(*this,"setStringAt")
			.push((uint32_t) row)
			.push((uint32_t) col)
			.push(text)
			.push((uint32_t) length)
			.call()
			.pop(rc);

		if(rc) {
            throw std::system_error((int) rc, std::system_category());
		}

		return *this;

	}

	TN3270::Session & IPC::Session::pfkey(unsigned short value) {

		int32_t rc;

		Request(*this,"pfkey")
			.push((uint32_t) value)
			.call()
			.pop(rc);

		if(rc) {
            throw std::system_error((int) rc, std::system_category());
		}

		return *this;
	}

	TN3270::Session & IPC::Session::pakey(unsigned short value) {

		int32_t rc;

		Request(*this,"pakey")
			.push((uint32_t) value)
			.call()
			.pop(rc);

		if(rc) {
            throw std::system_error((int) rc, std::system_category());
		}

		return *this;

	}

	TN3270::Session & IPC::Session::push(const Action action) {
		return this->action(toCharString(action));
	}

	TN3270::Session & IPC::Session::pop(int baddr, std::string &text) {

		Request(*this,"getFieldAtAddress")
			.push((uint32_t) baddr)
			.call()
			.pop(text);

		return *this;
	}

	TN3270::Session & IPC::Session::pop(int row, int col, std::string &text) {

		Request(*this,"getFieldAt")
			.push((uint32_t) row)
			.push((uint32_t) col)
			.call()
			.pop(text);

		return *this;
	}

	TN3270::Session & IPC::Session::pop(std::string &text) {

		Request(*this,"getFieldAtCursor")
			.call()
			.pop(text);

		return *this;

	}

	/// @brief Set cursor address.
	///
	/// @param addr	Cursor address.
	TN3270::Session & IPC::Session::setCursor(unsigned short addr) {

		setProperty("setCursorAddress", (uint32_t) addr);
		return *this;

	}

	unsigned short IPC::Session::getCursorAddress() {

		int32_t address;
		getProperty("cursor_address",address);
		return (unsigned short) address;

	}

	/// @brief Set cursor position.
	///
	/// @param row	New cursor row.
	/// @param col	New cursor column.
	TN3270::Session & IPC::Session::setCursor(unsigned short row, unsigned short col) {

		int32_t rc;

		Request(*this,"setCursorPosition")
			.push((uint32_t) row)
			.push((uint32_t) col)
			.call()
			.pop(rc);

		if(rc) {
            throw std::system_error((int) rc, std::system_category());
		}

		return *this;

	}

	void IPC::Session::getProperty(const char *name, int &value) const {

		Request(*this,false,name)
			.call()
			.pop(value);

	}

	void IPC::Session::setProperty(const char *name, const int value) const {

		int32_t rc;

		Request(*this,true,name)
			.push(value)
			.call()
			.pop(rc);

		if(rc) {
            throw std::system_error((int) rc, std::system_category());
		}

	}

	void IPC::Session::setProperty(const char *name, const char *value) const {

		int32_t rc;

		Request(*this,true,name)
			.push(value)
			.call()
			.pop(rc);

		if(rc) {
            throw std::system_error((int) rc, std::system_category());
		}

	}

	void IPC::Session::getProperty(const char *name, std::string &value) const {

		Request(*this,false,name)
			.call()
			.pop(value);

	}

	void IPC::Session::getProperty(const char *name, bool &value) const {
		throw std::system_error(ENOENT, std::system_category());
	}

	/// @brief Get lib3270 version.
	std::string IPC::Session::getVersion() const {

		string rc;
		getProperty("version",rc);
		return rc;

	}

	/// @brief Get lib3270 revision.
	std::string IPC::Session::getRevision() const {

		string rc;
		getProperty("revision",rc);
		return rc;

	}

	std::string IPC::Session::getLUName() const {

		string rc;
		getProperty("luname",rc);
		return rc;

	}

	/// @brief Execute action by name.
	TN3270::Session & IPC::Session::action(const char *action_name) {

		int32_t rc;

		Request(*this,"action")
			.push(action_name)
			.call()
			.pop(rc);

		if(rc) {
            throw std::system_error((int) rc, std::system_category());
		}

		return *this;
	}

	/// @brief Wait.
	TN3270::Session & IPC::Session::wait(unsigned short seconds) {

		time_t end = time(nullptr) + seconds;

		while(time(nullptr) < end) {

			sleep(1);
			if(getConnectionState() == TN3270::DISCONNECTED)
				throw std::runtime_error("Disconnected");

		}

		return *this;
	}

	/// @brief Wait for update.
	TN3270::Session & IPC::Session::waitForChange(unsigned short seconds) {

		int rc;

		time_t end = time(nullptr) + seconds;

		while(time(nullptr) < end) {

			debug("Running waitForUpdate request...");

			Request(*this,"waitForUpdate")
				.push((uint32_t) 1)
				.call()
				.pop(rc);

			debug("Wait for update returned ",rc);

			if(rc == 0)
				return *this;

		}

		throw std::system_error(ETIMEDOUT, std::system_category());

	}

	void IPC::Session::setUnlockDelay(unsigned short delay) {

		setProperty("unlock_delay", (uint32_t) delay);

	}

	std::string IPC::Session::getHostURL() const {

		std::string value;
		getProperty("url",value);
		return value;

	}

	void IPC::Session::setHostURL(const char *url) {
		setProperty("url",url);
	}

	unsigned short IPC::Session::getScreenWidth() const {

		int value;
		getProperty("width",value);
		return (unsigned short) value;

	}

	unsigned short IPC::Session::getScreenHeight() const {

		int value;
		getProperty("height",value);
		return (unsigned short) value;

	}

	unsigned short IPC::Session::getScreenLength() const {

		int value;
		getProperty("length",value);
		return (unsigned short) value;

	}

	TN3270::SSLState IPC::Session::getSSLState() const {

		int value;
		getProperty("sslstate",value);
		return (TN3270::SSLState) value;

	}

 }


