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

/*---[ Implement ]----------------------------------------------------------------------------------*/

std::string TN3270::Host::toString() const {

	this->session->waitForReady(this->timeout);
	return this->session->toString();
}

std::string TN3270::Host::toString(int baddr, int len, char lf) const {

	this->session->waitForReady(this->timeout);
	return this->session->toString(baddr,len,lf);

}

std::string TN3270::Host::toString(unsigned short row, unsigned short col, int len, char lf) const {

	this->session->waitForReady(this->timeout);
	return this->session->toString(row,col,len,lf);

}

/// @brief Checks if the terminal contains the string.
size_t TN3270::Host::find(const char * str, size_t pos) const {

	this->session->waitForReady(this->timeout);
	return this->session->find(str,pos);

}

/// @brief Get the number of occurrences of a string in the terminal.
size_t TN3270::Host::count(const char * str, size_t pos) const {

	this->session->waitForReady(this->timeout);
	return this->session->count(str,pos);

}

/// @brief Compare contents.
int TN3270::Host::compare(int baddr, const char* s, int len) const {

	this->session->waitForReady(this->timeout);
	return this->session->compare(baddr, s, len);

}

int TN3270::Host::compare(unsigned short row, unsigned short col, const char* s, int len) const {

	this->session->waitForReady(this->timeout);
	return this->session->compare(row,col,s,len);

}
