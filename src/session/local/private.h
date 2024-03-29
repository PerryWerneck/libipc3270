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

			class TN3270_PRIVATE Session : public TN3270::Abstract::Session {
			protected:

				/// @brief lib3270 handler.
				/// Do serialized calls to lib3270 methods.
				class Handler : protected std::mutex {
				protected:

					/// @brief Handle of the related instance of lib3270
					H3270 * hSession = NULL;

				public:

					Handler() {
					}

					virtual ~Handler() {
					}

					void chkResponse(int rc);
					void call(const std::function<int(H3270 * hSession)> &method);

					template <typename T>
					T get(const std::function<T (H3270 * hSession)> &method) {
						std::lock_guard<std::mutex> lock(*this);
						return method(hSession);
					}

				};

				std::shared_ptr<Handler> handler;

				/// @brief Timeout for automatic waits.
				time_t timeout;

				/// @brief Popup Handler.
				static int popupHandler(H3270 *session, const LIB3270_POPUP *popup, unsigned char wait);

				/// @brief Connect Handler.
				static void connectHandler(H3270 *session, unsigned char connected);

				/// @brief Wait for network events
				void wait(time_t timeout = 5);

				// Get strings from lib3270 without charset conversion.
				std::string	get() const override;
				std::string	get(int32_t baddr, int32_t len, uint8_t lf) const override;
				std::string	get(uint32_t row, uint32_t col, int32_t len, uint8_t lf) const override;

				// Set strings to lib3270 without charset conversion.
				void set(const std::string &str) override;
				void set(int32_t baddr, const std::string &str) override;
				void set(uint32_t row, uint32_t col, const std::string &str) override;

				Session(std::shared_ptr<Handler> handler);

			public:

				virtual ~Session();

				// Actions
				std::shared_ptr<TN3270::Action> ActionFactory(const LIB3270_ACTION *descriptor) override;

				// fields.
				std::shared_ptr<Field> FieldFactory(int baddr) override;
				std::shared_ptr<Field> FieldFactory(unsigned short row, unsigned short col) override;

				void connect(const char *url, time_t seconds) override;
				void disconnect() override;

				bool connected() const override;
				bool ready() const override;

				void pfkey(unsigned short value) override;
				void pakey(unsigned short value) override;
				void push(const KeyboardAction action) override;
				void print(LIB3270_CONTENT_OPTION option = LIB3270_CONTENT_ALL) override;

				void wait(time_t seconds) const override;
				void waitForReady(time_t timeout) const override;
				void waitForConnectionState(ConnectionState state, time_t timeout) const override;
				void waitForChange(time_t timeout) const override;
				void waitForConnected(time_t timeout) const override;

				LIB3270_KEYBOARD_LOCK_STATE waitForKeyboardUnlock(time_t seconds) const override;

				void wait(const char *text, int seconds) override;
				void wait(int addr, const char *text, int seconds) override;
				void wait(uint32_t row, uint32_t col, const char *text, int seconds) override;

				// States
				ProgramMessage getProgramMessage() const override;
				ConnectionState getConnectionState() const override;
				SSLState getSSLState() const override;

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
				unsigned short getCursorAddress() const override;
				Session::Cursor getCursorPosition() override;
				void setProperty(const char *name, const int value) override;
				void setProperty(const char *name, const unsigned int value) override;
				void setProperty(const char *name, const bool value) override;
				void setProperty(const char *name, const char *value) override;

			};

		}

	}

#endif // PRIVATE_H_INCLUDED
