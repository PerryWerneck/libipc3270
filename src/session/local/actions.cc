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
 #include <lib3270/actions.h>
 #include <lib3270/ipc/action.h>

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	std::shared_ptr<TN3270::Action> Local::Session::ActionFactory(const LIB3270_ACTION *descriptor) {

		class Action : public TN3270::Action {
		private:
			time_t timeout;
			std::shared_ptr<Handler> handler;

		public:
			Action(std::shared_ptr<Handler> h, time_t t, const LIB3270_ACTION *descriptor) : TN3270::Action{descriptor}, timeout{t}, handler{h} {
			}

			bool activatable() const override {

				return handler->get<bool>([this](H3270 * hSession) {
					return lib3270_action_is_activatable(descriptor,hSession) != 0;
				});

			}

			void activate() override {

				handler->call([this](H3270 * hSession){
					return lib3270_action_activate(descriptor,hSession);
				});

				if(timeout) {
					handler->call([this](H3270 * hSession){
						return lib3270_wait_for_ready(hSession,timeout);
					});
				}

			}

		};

		return std::make_shared<Action>(handler,timeout,descriptor);

	}

 	void Local::Session::connect(const char *url, time_t seconds) {

 		handler->call([this,url,seconds](H3270 * hSession){
			return lib3270_connect_url(hSession,url,seconds ? seconds : this->timeout);
 		});

	}

	void Local::Session::disconnect() {

 		handler->call([](H3270 * hSession){
			return lib3270_disconnect(hSession);
 		});

	}

	void Local::Session::pfkey(unsigned short value) {

 		handler->call([value](H3270 * hSession){
			return lib3270_pfkey(hSession,value);
 		});

 		if(this->timeout) {
			handler->call([this](H3270 * hSession){
				return lib3270_wait_for_ready(hSession,this->timeout);
			});
		}

	}

	void Local::Session::pakey(unsigned short value) {

 		handler->call([value](H3270 * hSession){
			return lib3270_pakey(hSession,value);
 		});

 		if(this->timeout) {
			handler->call([this](H3270 * hSession){
				return lib3270_wait_for_ready(hSession,this->timeout);
			});
		}

	}

	void Local::Session::push(const KeyboardAction id) {

		typedef int (*ActionCallback)(H3270 *);

		static const ActionCallback actions[] = {
            lib3270_enter,					// ENTER
            lib3270_erase,					// ERASE
            lib3270_eraseeof,				// ERASE_EOF
            lib3270_eraseeol,				// ERASE_EOL
            lib3270_eraseinput,				// ERASE_INPUT
            lib3270_kybdreset,				// KYBD_RESET
            lib3270_newline,				// NEWLINE
            lib3270_clear,					// CLEAR
            lib3270_select_field,			// SELECT_FIELD
            lib3270_select_all,				// SELECT_ALL
            lib3270_unselect,				// UNSELECT
            lib3270_reselect,				// RESELECT
            lib3270_delete,					// DELETE
            lib3270_dup,					// DUP
            lib3270_fieldmark,				// FIELDMARK
            lib3270_backspace,				// BACKSPACE
            lib3270_previousword,			// WORD_PREVIOUS
            lib3270_nextword,				// WORD_NEXT
            lib3270_fieldend,				// FIELD_END
            lib3270_firstfield,				// FIELD_FIRST
            lib3270_nextfield,				// FIELD_NEXT
            lib3270_previousfield,			// FIELD_PREVIOUS
            lib3270_attn,					// ATTN
            lib3270_break,					// BREAK
            lib3270_deleteword,				// WORD_DELETE
            lib3270_deletefield,			// FIELD_DELETE
            lib3270_sysreq,					// SYSREQ
            lib3270_clear_operator_error,	// KYBD_UNLOCK
		};

		if( ((size_t) id) > (sizeof(actions)/sizeof(actions[0]))) {
            throw std::system_error(EINVAL, std::system_category());
		}

		auto callback = actions[(size_t) id];

		handler->call([callback](H3270 * hSession){
			return callback(hSession);
 		});

 		if(this->timeout) {
			handler->call([this](H3270 * hSession){
				return lib3270_wait_for_ready(hSession,this->timeout);
			});
		}

	}

	void Local::Session::print(LIB3270_CONTENT_OPTION option) {

		handler->call([option](H3270 * hSession){

			switch(option) {
			case LIB3270_CONTENT_ALL:
				return lib3270_print_all(hSession);

			case LIB3270_CONTENT_COPY:
				return lib3270_print_copy(hSession);

			case LIB3270_CONTENT_SELECTED:
				return lib3270_print_selected(hSession);

			default:
				throw std::system_error(EINVAL, std::system_category());

			}

		});

	}

 }


