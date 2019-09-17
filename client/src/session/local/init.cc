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
 * @file src/session/local/init.cc
 *
 * @brief Implement lib3270 direct access layout (NO IPC).
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include "private.h"

#ifdef _WIN32
 #include <lmcons.h>
#endif // _WIN32

 extern "C" {
	 #include <lib3270/session.h>
 }


/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	Session * Local::getSessionInstance() {
		return new Local::Session();
	}

 #ifdef _WIN32

	string getUserName() {

		char	username[UNLEN + 1];
		DWORD	szName = sizeof(username);

		memset(username,0,UNLEN + 1);

		if(!GetUserName(username, &szName)) {
			return "?";
		}

		return string(username);

	}

	static void writeLog(HANDLE hEventLog, const char *msg, int rc = (int) GetLastError()) {


		debug(msg," rc=",rc);

		if(hEventLog) {

			string username = getUserName();
			char lasterror[1024];
			snprintf(lasterror,sizeof(lasterror),"The error code was %d",rc);

			const char *outMsg[] = {
				username.c_str(),
				msg,
				lasterror
			};

			ReportEvent(
				hEventLog,
				EVENTLOG_ERROR_TYPE,
				1,
				0,
				NULL,
				(sizeof(outMsg) / sizeof(outMsg[0])),
				0,
				outMsg,
				NULL
			);

		}

	}

	static string getInstallLocation(HANDLE hEventLog) {

		LSTATUS rc;
		HKEY hKey = 0;

		static const char * keys[] = {

			"Software\\" LIB3270_STRINGIZE_VALUE_OF(PRODUCT_NAME),
			"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\"  LIB3270_STRINGIZE_VALUE_OF(PRODUCT_NAME),

#ifdef LIB3270_NAME
			"Software\\" LIB3270_STRINGIZE_VALUE_OF(LIB3270_NAME),
			"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\"  LIB3270_STRINGIZE_VALUE_OF(LIB3270_NAME),
#endif

			"Software\\pw3270",
			"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\pw3270"
		};

		size_t ix;

		string installLocation;

		for(ix = 0; installLocation.empty() && ix < (sizeof(keys)/sizeof(keys[0])); ix++) {

			debug(ix,"=",keys[ix]);

			rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE,keys[ix],0,KEY_QUERY_VALUE,&hKey);
			if(rc == ERROR_SUCCESS) {

				unsigned long datatype; // #defined in winnt.h (predefined types 0-11)
				char datadir[4096];
				unsigned long datalen = sizeof(datadir);

				memset(datadir,0,datalen);

				rc = RegQueryValueExA(hKey,"InstallLocation",NULL,&datatype,(LPBYTE) datadir,&datalen);
				if(rc == ERROR_SUCCESS) {

					debug("Found: ",datadir);

					installLocation.assign(datadir);
					string username = getUserName();

					const char *outMsg[] = {
						username.c_str(),
						"Found LIB3270 installation",
						keys[ix],
						installLocation.c_str()
					};

					ReportEvent(
						hEventLog,
						EVENTLOG_INFORMATION_TYPE,
						1,
						0,
						NULL,
						(sizeof(outMsg) / sizeof(outMsg[0])),
						0,
						outMsg,
						NULL
					);

				}

				RegCloseKey(hKey);

			}

		}

		return installLocation;
	}

 #endif // _WIN32

 	Local::Session::Session() : Abstract::Session() {

		std::lock_guard<std::mutex> lock(sync);

#ifdef _WIN32
		{
			static bool initialized = false;

			if(!initialized) {

#ifdef LIB3270_NAME
				HANDLE hEventLog = RegisterEventSource(NULL, LIB3270_STRINGIZE_VALUE_OF(LIB3270_NAME));
#else
				HANDLE hEventLog = RegisterEventSource(NULL, PACKAGE_NAME);
#endif // LIB3270_NAME

				string installLocation = getInstallLocation(hEventLog);

				if(installLocation.empty()) {

					writeLog(hEventLog, "Can't identify lib3270 installation path");

				} else {

					// TODO: Understand why SetDefaultDllDirectories & AddDllDirectory causes failure in DNS resolution.


					SetCurrentDirectory(installLocation.c_str());

/*
					wchar_t	*path = (wchar_t *) malloc(sizeof(datadir)*sizeof(wchar_t));
					mbstowcs(path, datadir, 4095);

					if(!SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_USER_DIRS)) {
						writeLog(hEventLog,"SetDefaultDllDirectories has failed");
					}
#ifdef DEBUG
					else {
						debug("SetDefaultDllDirectories has suceeded");
					}
#endif

					if(!AddDllDirectory(path)) {
						string msg = "Can't add ";
						msg += datadir;
						msg += " to directory path";
						writeLog(hEventLog,msg.c_str());
					}
#ifdef DEBUG
					else {
						debug("AddDllDirectory has suceeded");
					}
#endif
					free(path);

*/
				}

				initialized = true;
				DeregisterEventSource(hEventLog);

			}

		}
#endif // _WIN32

		this->hSession = lib3270_session_new("");

		lib3270_set_user_data(this->hSession,(void *) this);
		setCharSet();
		lib3270_set_popup_handler(this->hSession, popupHandler);

		// Setup callbacks
		struct lib3270_session_callbacks *cbk;

		cbk = lib3270_get_session_callbacks(this->hSession,sizeof(struct lib3270_session_callbacks));
		if(!cbk) {
			throw runtime_error( _("Invalid callback table, possible version mismatch in lib3270") );
		}

		cbk->update_connect	= connectHandler;

	}

	Local::Session::~Session() {

		std::lock_guard<std::mutex> lock(sync);

		lib3270_session_free(this->hSession);
		this->hSession = nullptr;

	}

 }


