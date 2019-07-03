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

 #include "private.h"


/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	Host::Host(const char *id, const char *url, time_t timeout) {
		this->timeout = timeout;
		this->session = Session::create(id);
		if(url) {
			this->connect(url);
		}
	}

	Host::~Host() {
		delete this->session;
		this->session = nullptr;
	}

    void Host::connect(const char *url, bool sync) {
        this->session->connect(url);
        if(sync) {
			this->sync();
        }
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

    Host & Host::push(const Action action) {
        session->push(action);
        sync();
        return *this;
    }

	bool Host::isReady() const {
        this->session->waitForReady(this->timeout);
		return getProgramMessage() == MESSAGE_NONE;
	}

	bool Host::isConnected() const {
        this->session->waitForReady(this->timeout);
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


 }
