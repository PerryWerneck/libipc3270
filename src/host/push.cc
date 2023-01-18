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

TN3270::Host & TN3270::Host::push(int baddr, const char *str, int len) {
	session->push(baddr,str,len);
	return *this;
}

TN3270::Host & TN3270::Host::push(unsigned short row, unsigned short col, const char *str, int len) {
	session->push(row,col,str,len);
	return *this;
}

TN3270::Host & TN3270::Host::push(const char *str, int len) {
	session->push(str,len);
	return *this;
}

TN3270::Host & TN3270::Host::push(int baddr, const std::string &text) {
	session->push(baddr,text);
	return *this;
}

TN3270::Host & TN3270::Host::push(unsigned short row, unsigned short col, const std::string &text) {
	session->push(row,col,text);
	return *this;
}

TN3270::Host & TN3270::Host::push(const std::string &text) {
	session->push(text);
	return *this;
}

LIB3270_KEYBOARD_LOCK_STATE TN3270::Host::input(const char *text, int length, const char control_char) {
	return session->input(string(text,(size_t) length),control_char);
}

