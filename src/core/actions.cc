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
 * @file src/os/linux/actions.cc
 *
 * @brief Implements methods for static action management.
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include <ipc-client-internals.h>
 #include <lib3270/ipc.h>
 #include <lib3270/toggle.h>
 #include <lib3270/properties.h>
 #include <lib3270/ipc/action.h>
 #include <lib3270/ipc/session.h>
 #include <stdexcept>

 using namespace std;

 namespace TN3270 {

	TN3270_PUBLIC bool for_each(const std::function<bool(const LIB3270_ACTION &action)> &method) {

		for(const LIB3270_ACTION *action = lib3270_get_actions(); action->name; action++) {
			if(method(*action)) {
				return true;
			}
		}

		return false;
	}

	Action::~Action() {
	}

	bool Action::try_activate(time_t seconds) {

		if(!activatable()) {
			return false;
		}

		activate();

		if(seconds) {
			wait(seconds);
		}

		return true;
	}

	void Action::activate(time_t seconds) {

		activate();

		if(seconds) {
			wait(seconds);
		}

	}

	const char * Action::name() const noexcept {
		return ((const LIB3270_PROPERTY *) this->descriptor)->name;
	}

	const char * Action::description() const noexcept {
		return lib3270_property_get_description((const LIB3270_PROPERTY *) this->descriptor);
	}

	const char * Action::summary() const noexcept {
		return lib3270_property_get_summary((const LIB3270_PROPERTY *) this->descriptor);
	}

	std::shared_ptr<Action> Session::ActionFactory(const char *name) {

		for(auto action = lib3270_get_actions(); action->name; action++) {

			if(!strcasecmp(action->name,name)) {
				return ActionFactory(action);
			}

		}

		throw runtime_error("Invalid action name");

	}

 }

