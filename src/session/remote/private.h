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
 * @file private.h
 *
 * @brief Private definitions for local session.
 *
 * @author perry.werneck@gmail.com
 *
 */

#ifndef PRIVATE_H_INCLUDED

	#define PRIVATE_H_INCLUDED

	#ifdef HAVE_CONFIG_H
        #include <config.h>
	#endif // HAVE_CONFIG_H

	#include <ipc-client-internals.h>
	#include <lib3270/ipc/action.h>
	#include <private/session.h>
	#include <string>
	#include <lib3270.h>
	#include <stdexcept>

	using std::string;
	using std::runtime_error;

	namespace TN3270 {

		namespace IPC {

			class Session;

		}

	}

#endif // PRIVATE_H_INCLUDED
