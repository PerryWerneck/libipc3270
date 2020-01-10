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

#ifndef _WIN32
 #include <getopt.h>
 #pragma GCC diagnostic ignored "-Wunused-function"
#endif // _WIN32

#if defined(_MSC_VER)
	#pragma comment(lib,"ipc3270.lib")
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
 static void testAttributes(const char *session) {

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

 // test host object
 static void testHost(const char *session) {

	try {

		TN3270::Host host{session};

		cout
			<< "Version: " << host["version"]
			<< "\tRevision: " << host["Revision"]
			<< "\tConnected: " << host["Connected"]
			<< std::endl;

		host.connect(nullptr);

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

		host.wait(10);

		// host.input("test@0another line");

		host.push(TN3270::ENTER);
		host.wait(10);

		cout << host << endl;

		cout << endl << "[" << host.toString((unsigned int) 1, (unsigned int) 3,7) << "]" << endl;
		cout << endl << "[" << host.toString((int) 15, (int) 10) << "]" << endl;

		host.pfkey(3);
		host.wait(10);

		cout << host << endl;
		host.wait(10);

		host.disconnect();

	} catch(const std::exception &e) {

		cerr << std::endl << e.what() << std::endl << std::endl;

	}

 }

 int main(int argc, char **argv) {

	const char * session = ":a";

#ifndef _WIN32
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
	static struct option options[] = {
		{ "session",	required_argument,	0,	's' },
		{ 0, 0, 0, 0}

	};
	#pragma GCC diagnostic pop

	int long_index =0;
	int opt;
	while((opt = getopt_long(argc, argv, "s:", options, &long_index )) != -1) {

		switch(opt) {
		case 's':
			session = optarg;
			break;

		}

	}
#endif // _WIN32

	cout 	<< "Session: " << session << endl;

	testHost(session);



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


