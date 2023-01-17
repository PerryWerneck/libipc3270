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
 * Este programa está nomeado como service.c e possui - linhas de código.
 *
 * Contatos:
 *
 * perry.werneck@gmail.com	(Alexandre Perry de Souza Werneck)
 *
 */

#include "private.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ipc-glib.h>

#ifndef _WIN32
	#include <signal.h>
#endif // !_WIN32

GMainLoop * main_loop = NULL;

#ifdef DEBUG
	static gchar * pidfile = PACKAGE_NAME ".pid";
#else
	static gchar * pidfile = "/var/run/" PACKAGE_NAME ".pid";
#endif // DEBUG

#ifndef _WIN32
static void on_sigterm(int signal) {
	g_message("Stopping by termination request\n");
	g_main_loop_quit(main_loop);
}
#endif // !_WIN32

#if defined( HAVE_SYSLOG )
static void g_syslog(const gchar *log_domain,GLogLevelFlags log_level,const gchar *message,gpointer user_data)
{
 	static const struct _logtype
 	{
 		GLogLevelFlags	  log_level;
 		int 			  priority;
 		const gchar		* msg;
 	} logtype[] =
 	{
		{ G_LOG_FLAG_RECURSION,	LOG_INFO,		"recursion"			},
		{ G_LOG_FLAG_FATAL,		LOG_ERR,		"fatal error"		},

		/* GLib log levels */
		{ G_LOG_LEVEL_ERROR,	LOG_ERR,		"error"				},
		{ G_LOG_LEVEL_CRITICAL,	LOG_ERR,		"critical error"	},
		{ G_LOG_LEVEL_WARNING,	LOG_ERR,		"warning"			},
		{ G_LOG_LEVEL_MESSAGE,	LOG_ERR,		"message"			},
		{ G_LOG_LEVEL_INFO,		LOG_INFO,		"info"				},
		{ G_LOG_LEVEL_DEBUG,	LOG_DEBUG,		"debug"				},
 	};

	int f;

	for(f=0;f<G_N_ELEMENTS(logtype);f++)
	{
		if(logtype[f].log_level == log_level)
		{
			gchar *ptr;
			gchar *text = g_strdup_printf("%s: %s %s",logtype[f].msg,log_domain ? log_domain : "",message);
			for(ptr = text;*ptr;ptr++)
			{
				if(*ptr < ' ')
					*ptr = ' ';
			}

#ifdef DEBUG
			printf("%s\n",text);
			fflush(stdout);
#endif // DEBUG

			syslog(logtype[f].priority,"%s",text);
			g_free(text);
			return;
		}
	}

	#ifdef DEBUG
		printf("%s\n",message);
		fflush(stdout);
	#endif // DEBUG

	syslog(LOG_INFO,"%s %s",log_domain ? log_domain : "", message);
}
#endif // HAVE_SYSLOG

static gboolean do_idle_check(G_GNUC_UNUSED gpointer dunno) {

	return TRUE;
}

static void cleanup() {

	if(pidfile) {
		remove(pidfile);
	}

}

int main(int argc, char *argv[]) {

	static gboolean asDaemon = FALSE;

#if defined( HAVE_SYSLOG )
	openlog(g_get_prgname(), LOG_NDELAY, LOG_USER);
	g_log_set_default_handler(g_syslog,NULL);
#endif // HAVE_SYSLOG

	// Verifica argumentos
	static const GOptionEntry app_options[] = {
		{ "pidfile", 	'p', 0, G_OPTION_ARG_STRING, 	&pidfile,	"Path to pidfile" , NULL },
#ifndef _WIN32
		{ "daemon", 	'd', 0, G_OPTION_ARG_NONE,		&asDaemon,	"Run as daemon",	NULL },
#endif // !_WIN32
		{ NULL }
	};

	GOptionContext  * context	= g_option_context_new (PACKAGE_NAME " IPC service daemon");
	GError			* error		= NULL;
	GOptionGroup    * group		= g_option_group_new( PACKAGE_NAME, NULL, NULL, NULL, NULL);

	g_option_context_set_main_group(context, group);
	g_option_context_add_main_entries(context, app_options, NULL);
	if(!g_option_context_parse( context, &argc, &argv, &error )) {
		fprintf(stderr,"%s\n",error->message);
		return -1;
	}

	atexit(cleanup);

	if(pidfile) {
		FILE * hpid = fopen(pidfile,"w");
		if(hpid) {
			fprintf(hpid,"%u",(unsigned int) getpid());
			fclose(hpid);
		}
	}

	service_start();

	g_print("%s starts\n",argv[0]);

	main_loop = g_main_loop_new(NULL, FALSE);

#ifndef _WIN32

	if(asDaemon && daemon(0,0)) {
		g_print("%s can't start: %s\n",argv[0],strerror(errno));
		return -1;
	}

	signal(SIGTERM,on_sigterm);
	signal(SIGINT,on_sigterm);

#endif // !_WIN32

 	g_main_loop_run(main_loop);

	g_print("%s ends\n",argv[0]);

 	return 0;
}

