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
 * @file src/lib3270++/testprogram/testprogram.cc
 *
 * @brief
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include <getopt.h>
 #include <cstdlib>
 #include <lib3270/ipc.h>

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
			<< "Luname is " << hSession->getLUName() << std::endl
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

 // test host object
 static void testHost(const char *session) {

	TN3270::Host host{session,nullptr,10};

	try {

		cout
			<< "Version: " << host.getVersion()
			<< "\tRevision: " << host.getRevision()
			<< std::endl;

		host.connect(nullptr);

		cout
			<< "Connection state is " << toCharString(host.getConnectionState()) << std::endl
			<< "Program message is " << toCharString(host.getProgramMessage()) << std::endl
			<< "Luname is " << host.getLUName() << std::endl
			<< std::endl;

		if(host) {
			cout << host << endl;
		}

		// host.input("test@0another line");


	} catch(const std::exception &e) {

		cerr << std::endl << e.what() << std::endl << std::endl;

	}

 }

 int main(int argc, char **argv) {

	const char * session = "pw3270:a";

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

	cout << "Session: " << session << endl;
	testHost(session);

	return 0;
 }


