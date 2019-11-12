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
 * @brief
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include "private.h"
 #include <lib3270/properties.h>
 #include <lib3270/toggle.h>
 #include <cstring>

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	void IPC::Session::getAttribute(const char *name, int &value) const {

		Request(*this,false,name)
			.call()
			.pop(value);

	}

	void IPC::Session::getAttribute(const char *name, unsigned int &value) const {

		Request(*this,false,name)
			.call()
			.pop(value);

	}

	void IPC::Session::getAttribute(const char *name, std::string &value) const {

		Request(*this,false,name)
			.call()
			.pop(value);

	}

	void IPC::Session::getAttribute(const char GNUC_UNUSED(*name), bool GNUC_UNUSED(&value)) const {
		throw std::system_error(ENOTSUP, std::system_category());
	}

	void IPC::Session::setAttribute(const char *name, const int value) {

		int32_t rc;

		Request(*this,true,name)
			.push(value)
			.call()
			.pop(rc);

		if(rc) {
            throw std::system_error((int) rc, std::system_category());
		}

	}

	void IPC::Session::setAttribute(const char *name, const char *value) {

		int32_t rc;

		Request(*this,true,name)
			.push(value)
			.call()
			.pop(rc);

		if(rc) {
            throw std::system_error((int) rc, std::system_category());
		}

	}

	void IPC::Session::setCharSet(const char *charset) {
		// D-Bus calls are always UTF-8
		Abstract::Session::setCharSet("UTF-8",charset);
	}

	unsigned short IPC::Session::getScreenWidth() const {

		uint32_t value;
		getAttribute("width",value);
		return (unsigned short) value;

	}

	unsigned short IPC::Session::getScreenHeight() const {

		uint32_t value;
		getAttribute("height",value);
		return (unsigned short) value;

	}

	unsigned short IPC::Session::getScreenLength() const {

		uint32_t value;
		getAttribute("length",value);
		return (unsigned short) value;

	}

	void IPC::Session::setUnlockDelay(unsigned short delay) {
		setAttribute("unlock_delay", (uint32_t) delay);
	}

	void IPC::Session::setLockOnOperatorError(bool lock) {
		setAttribute("oerrlock", (uint32_t) lock);
	}

	unsigned short IPC::Session::setCursor(int addr) {

		int32_t rc;

		Request(*this,"setCursorAddress")
			.push((int32_t) addr)
			.call()
			.pop(rc);

		if(rc < 0)
			chkResponse(-rc);

		return (unsigned short) rc;

	}

	unsigned short IPC::Session::setCursor(unsigned short row, unsigned short col) {

		int32_t rc;

		Request(*this,"setCursorPosition")
			.push((uint32_t) row)
			.push((uint32_t) col)
			.call()
			.pop(rc);

		if(rc < 0)
			chkResponse(-rc);

		return (unsigned short) rc;

	}

	unsigned short IPC::Session::getCursorAddress() {

		int32_t address;
		getAttribute("cursor_address",address);
		return (unsigned short) address;

	}

	std::string IPC::Session::getVersion() const {

		string rc;
		getAttribute("version",rc);
		return rc;

	}

	std::string IPC::Session::getRevision() const {

		string rc;
		getAttribute("revision",rc);
		return rc;

	}

	std::string IPC::Session::getAssociatedLUName() const {

		string rc;
		getAttribute("associated_lu",rc);
		return rc;

	}

	std::string IPC::Session::getHostURL() const {

		std::string value;
		getAttribute("url",value);
		return value;

	}

	void IPC::Session::setHostURL(const char *url) {

		setAttribute("url",url);

	}

 }


