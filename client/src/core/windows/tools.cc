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
 * @brief Win32 convenience methods.
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include <ipc-client-internals.h>
 #include <lmcons.h>
 #include <fcntl.h>

#if defined(_MSC_VER)
	#pragma comment(lib,"Advapi32.lib")
#endif // _MSC_VER

 using std::string;

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	std::string getUserName() {

		char	username[UNLEN + 1];
		DWORD	szName = sizeof(username);

		memset(username,0,UNLEN + 1);

		if(!GetUserName(username, &szName)) {
			return "";
		}

		return std::string(username);

	}

	std::string getInstallLocation() {

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

				}

				RegCloseKey(hKey);

			}

		}

		return installLocation;
	}

 }


