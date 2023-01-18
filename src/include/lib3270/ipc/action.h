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
 * @file lib3270/ipc/action.h
 *
 * @brief Declares the TN3270 IPC Action object.
 *
 * @author perry.werneck@gmail.com
 *
 */

#ifndef IPC3270_ACTION_H_INCLUDED

	#define IPC3270_ACTION_H_INCLUDED

	#include <lib3270/ipc.h>
	#include <string.h>

	namespace TN3270 {

		/// @brief TN3270 Action
		class TN3270_PUBLIC Action {
		protected:
			const LIB3270_ACTION *descriptor;

			Action(const LIB3270_ACTION *descriptor);

		public:
			virtual bool activatable() const = 0;
			virtual void activate() = 0;
			virtual void wait(time_t seconds = 0) = 0;
			virtual ~Action();

			inline operator bool() const {
				return activatable();
			}

			const char * getName() const noexcept;
			const char * getDescription() const noexcept;
			const char * getSummary() const noexcept;

		};

	}

#endif // IPC3270_ACTION_H_INCLUDED
