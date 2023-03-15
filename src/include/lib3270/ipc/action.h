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
		private:
			/// @brief Default timeout value.
			time_t timeout;

		protected:
			const LIB3270_ACTION *descriptor;

			constexpr Action(const LIB3270_ACTION *dsc, time_t maxwait = DEFAULT_TIMEOUT) : timeout{maxwait}, descriptor{dsc}{
			}

		public:

			/// @brief Check if the action can be activated.
			/// @return true if the action can be activated.
			virtual bool activatable() const = 0;

			/// @brief Activate action.
			virtual void activate() = 0;

			/// @brief Wait for terminal to be ready (convenience method).
			/// @param seconds Maximum time to wait.
			virtual void wait(time_t seconds) = 0;

			/// @brief Wait for terminal to be ready using default timeout.
			inline void wait() {
				wait(this->timeout);
			}

			/// @brief Activate action and wait for ready.
			void activate(time_t seconds);

			/// @brief Activate action if available, do nothing if not available.
			/// @param seconds Seconds to wait for ready if activated, 0 to disable (@see wait).
			/// @return true if the action was activated, false if not.
			bool try_activate(time_t seconds = 0);

			virtual ~Action();

			inline operator bool() const {
				return activatable();
			}

			inline time_t maxwait() const noexcept {
				return this->timeout;
			}

			inline void maxwait(time_t value) noexcept {
				this->timeout = value;
			}

			const char * name() const noexcept;
			const char * description() const noexcept;
			const char * summary() const noexcept;

		};

	}

#endif // IPC3270_ACTION_H_INCLUDED
