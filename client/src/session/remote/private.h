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

		namespace IPC {

			class TN3270_PRIVATE Session : public TN3270::Abstract::Session {
			private:

				friend class Request;

#ifdef _WIN32
				/// @brief Pipe Handle.
				HANDLE hPipe;

				/// @brief IPC Data type.
				enum Type : uint8_t {
					String	= 's',
					Boolean	= 'b',
					Uchar	= 'y',
					Int16	= 'n',
					Uint16	= 'q',
					Int32	= 'i',
					Int32x	= 'h',
					Uint32	= 'u',
					Int64	= 'x',
					Uint64	= 't'
				};

				struct {
					DWORD 	  length;	///< @brief Length of input buffer.
					DWORD	  used;		///< @brief Length of used block.
					DWORD	  current;	///< @brief Offset of the current argument.
					uint8_t * block;
				} in;

				struct {
					DWORD 	  length;
					DWORD	  used;
					uint8_t * block;
				} out;

				struct DataBlock {
					Type type;
				};

				/// @brief Store value on data block.
				DataBlock * pushBlock(const void *ptr, size_t len);

				/// @brief Get next argument.
				DataBlock * getNextBlock() const;

#else
				struct {
					DBusMessage		* in;
					DBusMessage		* out;
					DBusMessageIter	  iter;

				} msg;
				DBusConnection	* conn;

#endif // _WIN32

//				Request(const Session &session);

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
				void action(const char *action_name) override;
				void connect(const char *url, int seconds) override;
				void disconnect() override;
				void pfkey(unsigned short value) override;
				void pakey(unsigned short value) override;
				void push(const Action action) override;
				void print(LIB3270_CONTENT_OPTION option = LIB3270_CONTENT_ALL) override;

				void wait(unsigned short seconds) const override;
				void waitForReady(time_t timeout) const override;
				void waitForChange(unsigned short seconds) const override;

				// States
				ProgramMessage getProgramMessage() const override;
				ConnectionState getConnectionState() const override;
				SSLState getSSLState() const override;

				// Properties.
				void getProperty(const char *name, int &value) const override;
				void getProperty(const char *name, std::string &value) const override;
				void getProperty(const char *name, bool &value) const override;
				std::string getVersion() const override;
				std::string getRevision() const override;
				std::string getLUName() const override;

				std::string getHostURL() const override;
				void setHostURL(const char *url) override;

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
