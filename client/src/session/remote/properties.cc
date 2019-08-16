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

	void IPC::Session::getProperty(const char *name, int &value) const {
	}

	void IPC::Session::getProperty(const char *name, std::string &value) const {
	}

	void IPC::Session::getProperty(const char *name, bool &value) const {
	}

	void IPC::Session::setCharSet(const char *charset) {
	}

	unsigned short IPC::Session::getScreenWidth() const {
	}

	unsigned short IPC::Session::getScreenHeight() const {
	}

	unsigned short IPC::Session::getScreenLength() const {
	}

	void IPC::Session::setUnlockDelay(unsigned short delay) {
	}

	void IPC::Session::setCursor(unsigned short addr) {
	}

	void IPC::Session::setCursor(unsigned short row, unsigned short col) {
	}

	unsigned short IPC::Session::getCursorAddress() {
	}

	std::string IPC::Session::getVersion() const {
	}

	std::string IPC::Session::getRevision() const {
	}

	std::string IPC::Session::getLUName() const {
	}

	std::string IPC::Session::getHostURL() const {
	}

	void IPC::Session::setHostURL(const char *url) {
	}

 }


