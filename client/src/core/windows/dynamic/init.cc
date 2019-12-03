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
 * @brief Win32 delayed loading support methods.
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include <ipc-client-internals.h>
 #include <delayimp.h>
 #include <fcntl.h>

#if defined(_MSC_VER)
	#pragma comment(lib,"DelayImp.lib")
#endif // _MSC_VER

 extern "C" {

//	extern __declspec (dllexport) PfnDliHook __pfnDliNotifyHook2;
//	extern __declspec (dllexport) PfnDliHook __pfnDliFailureHook2;

	FARPROC WINAPI IPC3270_DelayLoadHook(unsigned reason, DelayLoadInfo * info);

 }

 using std::string;

/*---[ Implement ]----------------------------------------------------------------------------------*/

 // https://docs.microsoft.com/en-us/cpp/build/reference/loading-all-imports-for-a-delay-loaded-dll?view=vs-2019

#if defined(_MSC_VER)
 const PfnDliHook __pfnDliNotifyHook2 = IPC3270_DelayLoadHook;
 const PfnDliHook __pfnDliFailureHook2 = IPC3270_DelayLoadHook;
#else
 PfnDliHook __pfnDliNotifyHook2 = IPC3270_DelayLoadHook;
 PfnDliHook __pfnDliFailureHook2 = IPC3270_DelayLoadHook;
#endif // _MSC_VER

 static HANDLE hModule = 0;
 static HANDLE hEventLog = 0;

 BOOL WINAPI DllMain(HANDLE hInstance, DWORD dwcallpurpose, LPVOID lpvResvd) {

#ifdef HAVE_LIBINTL
 	static bool initialized = false;
#endif // HAVE_LIBINTL

    switch(dwcallpurpose) {
    case DLL_PROCESS_ATTACH:
        hModule = hInstance;
        hEventLog = RegisterEventSource(NULL, LIB3270_STRINGIZE_VALUE_OF(PRODUCT_NAME));

#ifdef HAVE_LIBINTL
		if(!initialized) {
			initialized = true;

			std::string localedir = TN3270::getInstallLocation();
			localedir += "\\locale";

			bindtextdomain(PACKAGE_NAME, localedir.c_str());
		}
#endif // HAVE_LIBINTL

        break;

	case DLL_PROCESS_DETACH:
		hModule = 0;
		DeregisterEventSource(hEventLog);
		hEventLog = 0;
		break;

    }

    return TRUE;
 }

 static void eventlog(const char *msg) {

        std::string username = TN3270::getUserName();

        const char *outMsg[] = {
                username.c_str(),
                PACKAGE_NAME,
                msg
        };

        debug("username"," ",msg);

        ReportEvent(
                hEventLog,
                EVENTLOG_ERROR_TYPE,
                1,
                0,
                NULL,
                3,
                0,
                outMsg,
                NULL
        );

 }

 static void dummyProc() {
 	debug(__FUNCTION__," was called");
	throw std::runtime_error("Operation not supported");
 }

 FARPROC WINAPI IPC3270_DelayLoadHook(unsigned reason, DelayLoadInfo * info) {

	static string savedpath;

	// https://docs.microsoft.com/en-us/cpp/build/reference/structure-and-constant-definitions?view=vs-2019

	debug(__FUNCTION__," ",(const char *) info->szDll);

	switch (reason) {
	case dliNoteStartProcessing:
		debug(__FUNCTION__,"::dliNoteStartProcessing")
		{
			string installLocation = TN3270::getInstallLocation();

			if(!installLocation.empty()) {

				char curdir[4096];
				memset(curdir,0,sizeof(curdir));

				if(GetCurrentDirectory(sizeof(curdir)-1,curdir)) {
					savedpath = curdir;
					debug("Saved path: \"",savedpath,"\"");
				} else {
					savedpath.clear();
				}

				SetCurrentDirectory(installLocation.c_str());

				debug("Current path changed to \"",installLocation,"\"");
			}
		}
		break;

	case dliNoteEndProcessing:
		debug(__FUNCTION__,"::dliNoteEndProcessing")

		if(!savedpath.empty()) {
			debug("Restoring current path to \"",savedpath,"\"");
			SetCurrentDirectory(savedpath.c_str());
			savedpath.clear();

		}

		break;

	case dliNotePreLoadLibrary:
		debug(__FUNCTION__,"::dliNotePreLoadLibrary");
		break;

	case dliNotePreGetProcAddress:
		debug(__FUNCTION__,"::dliNotePreGetProcAddress")
		break;

	case dliFailLoadLib:
		debug(__FUNCTION__,"::dliFailLoadLib")
		{
			string msg = "Can't load ";
			msg += (const char *) info->szDll;
			eventlog(msg.c_str());
		}
		return (FARPROC) hModule;

	case dliFailGetProc:
		debug(__FUNCTION__,"::dliFailGetProc");
		{
			string msg = "Can't find method on ";
			msg += (const char *) info->szDll;
			eventlog(msg.c_str());
		}
		return (FARPROC) dummyProc;

	}

	// Returning NULL causes the delay load machinery to perform default
	// processing for this notification.
	return NULL;

 }

