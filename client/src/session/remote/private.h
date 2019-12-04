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
	#include <string>
	#include <lib3270.h>
	#include <stdexcept>

	using std::string;
	using std::runtime_error;

	namespace TN3270 {

		namespace IPC {

			class Session;

			class Action : public TN3270::Action {
			private:
				Session *session;

			public:
				Action(Session *session, const LIB3270_ACTION *descriptor);
				bool activatable() const override;
				void activate() override;
				void wait(time_t seconds) override;

			};

			class TN3270_PRIVATE Session : public TN3270::Abstract::Session {
			private:

				friend class Request;

#ifdef _WIN32
				/// @brief Pipe Handle.
				HANDLE hPipe;
#else

				DBusConnection	* conn;
				std::string		  name;			///< @brief D-Bus Object name.
				std::string		  path;			///< @brief D-Bus Object path.
				std::string		  interface;	///< @brief D-Bus interface.

#endif // _WIN32

				void wait(int seconds, std::function<int()> worker) const;

			protected:

				// Get strings from lib3270 without charset conversion.
				std::string	get() const override;
				std::string	get(int baddr, int len, char lf) const override;
				std::string	get(unsigned int row, unsigned int col, int len, char lf) const override;

				// Set strings to lib3270 without charset conversion.
				void set(const std::string &str) override;
				void set(int baddr, const std::string &str) override;
				void set(int row, int col, const std::string &str) override;

			public:

				Session(const char *id, const char *charset = nullptr);
				virtual ~Session();

				// Actions
				TN3270::Action * getAction(const LIB3270_ACTION *descriptor) override;

				// Actions
				void action(const char *action_name) override;
				void connect(const char *url, int seconds) override;
				void disconnect() override;
				void pfkey(unsigned short value) override;
				void pakey(unsigned short value) override;
				void push(const KeyboardAction action) override;
				void print(LIB3270_CONTENT_OPTION option = LIB3270_CONTENT_ALL) override;

				void wait(time_t seconds) const override;
				void waitForReady(time_t timeout) const override;
				void waitForChange(time_t timeout) const override;

				LIB3270_KEYBOARD_LOCK_STATE waitForKeyboardUnlock(time_t seconds) const override;

				void wait(const char *text, int seconds) override;
				void wait(int addr, const char *text, int seconds) override;
				void wait(unsigned short row, unsigned short col, const char *text, int seconds) override;

				// States
				ProgramMessage getProgramMessage() const override;
				ConnectionState getConnectionState() const override;
				SSLState getSSLState() const override;

				// Properties.
				void getAttribute(const char *name, int &value) const override;
				void getAttribute(const char *name, unsigned int &value) const override;
				void getAttribute(const char *name, std::string &value) const override;
				void getAttribute(const char *name, bool &value) const override;

				void setAttribute(const char *name, const int value) override;
				void setAttribute(const char *name, const char *value) override;

				// Attributes
				Attribute getAttribute(const char *name) const override;
				void getAttributes(std::vector<Attribute> & attributes) const override;

				std::string getVersion() const override;
				std::string getRevision() const override;
				std::string getAssociatedLUName() const override;
				LIB3270_KEYBOARD_LOCK_STATE getKeyboardLockState() const override;

				std::string getHostURL() const override;
				void setHostURL(const char *url) override;

				unsigned short getScreenWidth() const override;
				unsigned short getScreenHeight() const override;
				unsigned short getScreenLength() const override;
				void setUnlockDelay(unsigned short delay) override;
				void setLockOnOperatorError(bool lock) override;
				void setCharSet(const char *charset = NULL) override;
				unsigned short setCursor(int addr) override;
				unsigned short setCursor(unsigned short row, unsigned short col) override;
				unsigned short getCursorAddress() override;

			};

		}

	}

#endif // PRIVATE_H_INCLUDED
