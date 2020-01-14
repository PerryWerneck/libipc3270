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
 * @brief Linux dynamic module initialization.
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include <ipc-client-internals.h>

 extern "C" {

	int ipc3270_loaded(void) __attribute__((constructor));
	int ipc3270_unloaded(void) __attribute__((destructor));

 }

/*---[ Implement ]----------------------------------------------------------------------------------*/

 int ipc3270_loaded(void) {

	debug("LOCALEDIR=\"%s\"",LIB3270_STRINGIZE_VALUE_OF(LOCALEDIR));
	bindtextdomain(GETTEXT_PACKAGE, LIB3270_STRINGIZE_VALUE_OF(LOCALEDIR));

	return 0;

 }

 int ipc3270_unloaded(void) {

	return 0;

 }

