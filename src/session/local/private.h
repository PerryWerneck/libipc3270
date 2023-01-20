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
	#include <lib3270/popup.h>
	#include <stdexcept>

	using std::string;
	using std::runtime_error;

	namespace TN3270 {

		namespace Local {

			class Session;

			class Action : public TN3270::Action {
			private:
				Session *session;

			public:
				Action(Session *hSession, const LIB3270_ACTION *descriptor);
				bool activatable() const noexcept override;
				void activate() override;
				void wait(time_t seconds) override;

			};

			class TN3270_PRIVATE Session : public TN3270::Abstract::Session {
			private:

				friend class Action;

				/// @brief Timeout for automatic waits.
				time_t timeout;

				/// @brief Handle of the related instance of lib3270
				H3270 * hSession;

				/// @brief Recursive mutex to serialize access to lib3270
				std::recursive_mutex sync;

				/// @brief Popup Handler.
				static int popupHandler(H3270 *session, const LIB3270_POPUP *popup, unsigned char wait);

				/// @brief Connect Handler.
				static void connectHandler(H3270 *session, unsigned char connected);

				/// @brief Wait for network events
				void wait(time_t timeout = 5);

			protected:

				// Get strings from lib3270 without charset conversion.
				std::string	get() const override;
				std::string	get(int32_t baddr, int32_t len, uint8_t lf) const override;
				std::string	get(uint32_t row, uint32_t col, int32_t len, uint8_t lf) const override;

				// Set strings to lib3270 without charset conversion.
				void set(const std::string &str) override;
				void set(int baddr, const std::string &str) override;
				void set(int row, int col, const std::string &str) override;

			public:

				Session(const char *charset = nullptr);
				virtual ~Session();

				// Actions
				TN3270::Action * getAction(const LIB3270_ACTION *descriptor) override;

				void action(const char *action_name) override;
				void connect(const char *url, time_t seconds) override;
				void disconnect() override;
				void pfkey(unsigned short value) override;
				void pakey(unsigned short value) override;
				void push(const KeyboardAction action) override;
				void print(LIB3270_CONTENT_OPTION option = LIB3270_CONTENT_ALL) override;

				void wait(time_t seconds) const override;
				void waitForReady(time_t timeout) const override;
				void waitForConnectionState(ConnectionState state, time_t timeout) const override;
				void waitForChange(time_t timeout) const override;

				LIB3270_KEYBOARD_LOCK_STATE waitForKeyboardUnlock(time_t seconds) const override;

				void wait(const char *text, int seconds) override;
				void wait(int addr, const char *text, int seconds) override;
				void wait(unsigned short row, unsigned short col, const char *text, int seconds) override;

				// States
				ProgramMessage getProgramMessage() const override;
				ConnectionState getConnectionState() const override;
				SSLState getSSLState() const override;

				// Attributes
				//std::shared_ptr<Abstract::Attribute> getAttribute(const char *name) const override;
				//void getAttributes(std::vector<std::shared_ptr<Abstract::Attribute>> & attributes) const override;

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
				void setTimeout(time_t timeout) override;
				void setLockOnOperatorError(bool lock) override;
				void setCharSet(const char *charset = NULL) override;
				unsigned short setCursor(int addr) override;
				unsigned short setCursor(unsigned short row, unsigned short col) override;
				unsigned short getCursorAddress() override;
				Session::Cursor getCursorPosition() override;
				void setProperty(const char *name, const int value) override;
				void setProperty(const char *name, const unsigned int value) override;
				void setProperty(const char *name, const bool value) override;
				void setProperty(const char *name, const char *value) override;

			};

		}

	}

#endif // PRIVATE_H_INCLUDED
