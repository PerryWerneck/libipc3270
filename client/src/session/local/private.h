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

	#include <config.h>
	#include <ipc-client-internals.h>
	#include <string>
	#include <lib3270.h>
	#include <stdexcept>

	using std::string;
	using std::runtime_error;

	namespace TN3270 {

		namespace Local {

			class TN3270_PRIVATE Session : public TN3270::Abstract::Session {
			private:

				/// @brief Handle of the related instance of lib3270
				H3270 * hSession;

				/// @brief Mutex to serialize access to lib3270
				std::mutex sync;

				/// @brief Popup Handler.
				static void popupHandler(H3270 *session, LIB3270_NOTIFY type, const char *title, const char *msg, const char *fmt, va_list arg);

				/// @brief Connect Handler.
				static void connectHandler(H3270 *session, unsigned char connected);

				/// @brief Wait for network events
				void wait(time_t timeout = 5);

				/// @brief Check lib3270 return codes, launch exception when failed.
				static void chkResponse(int rc);

			protected:

				// Get strings from lib3270 without charset conversion.
				std::string	get() const override;
				std::string	get(int baddr, size_t len, char lf) const override;
				std::string	get(int row, int col, size_t sz, char lf) const override;

				// Set strings to lib3270 without charset conversion.
				void set(const std::string &str) override;
				void set(int baddr, const std::string &str) override;
				void set(int row, int col, const std::string &str) override;

			public:

				Session();
				virtual ~Session();

				// Actions
				void connect(const char *url, bool wait) override;
				void disconnect() override;
				void pfkey(unsigned short value) override;
				void pakey(unsigned short value) override;

				void wait(unsigned short seconds) const override;
				void waitForReady(time_t timeout) const override;
				void waitForChange(unsigned short seconds) const override;

				// States
				ProgramMessage getProgramMessage() const override;
				ConnectionState getConnectionState() const override;
				SSLState getSSLState() const override;

				// Properties.
				std::string getVersion() const override;
				std::string getRevision() const override;
				std::string getLUName() const override;
				std::string getHostURL() const override;
				unsigned short getScreenWidth() const override;
				unsigned short getScreenHeight() const override;
				unsigned short getScreenLength() const override;
				void setUnlockDelay(unsigned short delay) override;
				void setCharSet(const char *charset = NULL) override;
				void setCursor(unsigned short addr) override;
				void setCursor(unsigned short row, unsigned short col) override;
				unsigned short getCursorAddress() override;


			};

		}

	}

#endif // PRIVATE_H_INCLUDED
