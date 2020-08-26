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
 * @file client/src/testprogram/testprogram.cc
 *
 * @brief
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include <ctime>

#if defined(_MSC_VER)
	#pragma comment(lib,"ipc3270.lib")
#else
	#pragma GCC diagnostic ignored "-Wunused-function"
	#pragma GCC diagnostic ignored "-Wunused-parameter"
	#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"

	#include <getopt.h>

#endif // _MSC_VER

 #include <cstdlib>
 #include <lib3270.h>
 #include <lib3270/ipc.h>
 #include <lib3270/ipc/action.h>

 using namespace std;

/*---[ Implement ]----------------------------------------------------------------------------------*/

/*
 // Test "Session" object
 static void chkSession() {

	TN3270::Session * hSession = TN3270::Session::getInstance();

	try {

		cout
			<< "Version: " << hSession->getVersion()
			<< "\tRevision: " << hSession->getRevision()
			<< endl;

		hSession->connect("tn3270s://3270.df.bb:9023");
		hSession->waitForReady(30);

		cout
			<< "Connection state is " << toCharString(hSession->getConnectionState()) << std::endl
			<< "Program message is " << toCharString(hSession->getProgramMessage()) << std::endl
			<< "Luname is " << hSession->getAssociatedLUName() << std::endl
			<< std::endl;

		cout
			<< endl << endl
			<< hSession->toString()
			<< endl << endl;

		hSession->disconnect();

	} catch(const std::exception &e) {

		cerr << "Exception: " << e.what() << endl;

	}

	delete hSession;

 }
 */

 // Test Attributes
 static void testAttributes(const char *session, const char *url) {

	TN3270::Host host{session};

	for(auto attribute : host.getAttributes()) {

		cout << attribute.getName() << ":\t";

		try {

			cout << attribute.toString();

		} catch(const std::exception &e) {

			cout << e.what();
		}

		cout << endl;

	}

 }

 // Performance test.
 static void testPerformance(const char *session, const char *url) {

	try {

		TN3270::Host host{session};

		host.connect();

		// Check "WaitForReady"
		{
			time_t start = time(0);

			host.waitForReady(5);
			cout << "First run:" << (time(0) - start) << endl;

			for(size_t ix=0;ix<100;ix++) {
				host.waitForReady(1);
			}
			cout << "Time for 100 iterations: " << (time(0) - start) << endl;

		}

		// Check "Get"
		{
			time_t start = time(0);

			for(size_t ix=0;ix<100;ix++) {
				host.toString(14,1,75,0);
			}
			cout << "Time for 100 iterations: " << (time(0) - start) << endl;

		}


	} catch(const std::exception &e) {

		cerr << std::endl << e.what() << std::endl << std::endl;

	}

 }

 // test host object
 static void testHost(const char *session, const char *url) {

	try {

		TN3270::Host host{session};

		cout
			<< "Version: " << host["version"]
			<< "\tRevision: " << host["Revision"]
			<< "\tConnected: " << host["Connected"]
			<< std::endl;

		host.setUnlockDelay(0);	// Disable the 350ms delay on screen changes.
		host.setTimeout(10);	// Set the default timeout.
		host["crlget"] = false;	// Disable CRL get to speed up the connection.
		host.connect(url);

		cout
			<< "Wait for unlock returns " << host.getKeyboardLockState() << std::endl
			<< "Connection state is " << toCharString(host.getConnectionState()) << std::endl
			<< "Program message is " << toCharString(host.getProgramMessage()) << std::endl
			<< "Associated luname is " << host.getAssociatedLUName() << std::endl
			<< "Screen size is " << host.getScreenWidth() << "x" << host.getScreenHeight() << " (" << host.getScreenLength() << ")"
			<< std::endl;

		if(host) {
			cout << host << endl;
		}

		host.setCursor(10,10);
//		host.wait(10);
		// host.input("test@0another line");

		cout << "Sending ENTER" << endl;
		host.push(TN3270::ENTER);
		cout << "ENTER returns" << endl;

		host.wait(2);

		cout << host << endl;

		cout << endl << "[" << host.toString((unsigned int) 1, (unsigned int) 3,7) << "]" << endl;
		cout << endl << "[" << host.toString((int) 15, (int) 10) << "]" << endl;

		cout << "Sending PF3" << endl;
		host.pfkey(3);
		cout << "PF3 returns" << endl;

		cout << host << endl;

		cout << "Disconnecting" << endl;
		host.disconnect();

		cout << "Test complete" << endl;

	} catch(const std::exception &e) {

		cerr << std::endl << e.what() << std::endl << std::endl;

	}

 }

 int main(int argc, char **argv) {

	const char * session = ":A";
	const char * url = nullptr;

#if ! defined(_MSC_VER)

	static struct option options[] = {
		{ "session",	required_argument,	0,	's' },
		{ "url",		required_argument,	0,	'U' },
		{ "perftest",	no_argument,		0,	'P' },
		{ "info",		no_argument,		0,	'I' },
		{ 0, 0, 0, 0}

	};

	int long_index =0;
	int opt;
	while((opt = getopt_long(argc, argv, "s:", options, &long_index )) != -1) {

		switch(opt) {
		case 's':
			session = optarg;
			cout << "Session: " << session << endl;
			break;

		case 'U':
			url = optarg;
			cout << "URL: " << session << endl;
			break;

		case 'P':
			testPerformance(session,url);
			return 0;

		case 'I':
			testHost(session,url);
			return 0;

		}

	}

#else

		printf("\nRunning IPC Client tests\n");

		TN3270::Host host{session};
		host.setTimeout(10);
		host.connect();
		printf("\n\nWaiting...\n");
		host.wait(14,22,"SISTEMA");

		host.disconnect();

#endif // !_MSC_VER

		/*

		host.setTimeout(10);

		host.connect();
		host.push(TN3270::ENTER);

		host.toString(14,1,75,0);

		// host.disconnect();
		*/


	// cout 	<< "Session: " << session << endl;

	//testHost(session);
	// testPerformance(session);


	//testAttributes(session);

	/*
	{
		TN3270::Host host{":a",nullptr,10};

		cout << "pre: " << host["url"] << endl;

		host["url"] = "http://www.google.com.br";

		cout << "post: " << host["url"] << endl;
	}
	*/

	/*
	{
		TN3270::Host host{":a",nullptr,10};

		TN3270::Action * action = host.getAction("reconnect");

        cout << "Reconnect is " << (action->activatable() ? "available" : "unavailable") << endl;

		delete action;
	}
	*/

	/*
	for(auto attribute : TN3270::getAttributes()) {

        cout << attribute->name << ":\t" << attribute->description << endl;

	}
	*/

	return 0;
 }


