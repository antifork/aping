/*
 * $Id$
 *
 * New aping.
 * 
 * Copyright (c) 2002 Nicola Bonelli <bonelli@antifork.org>
 *                    Roberto Ferranti <sbirish@sbirish.net>
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. 2.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <signal.h>

#include "config.h"

/* plugin headers */

#include "header.h"
#include "plugin.h"
#include "../../version.h"

/* Thanks to ettercap's authors */
#ifdef __OpenBSD__
// The below define is a lie since we are really doing RTLD_LAZY since the
// system doesn't support RTLD_NOW.
#define RTLD_NOW DL_LAZY
#endif



long          (*plugin_init) (void);
long          (*plugin_exit) (void);
char         *plugin_name;
char         *plugin_string;
char         *plugin_traname;
char         *plugin_version;
char         *plugin_bugrep;
char	     *plugin_info;

void
handler_usr1 (int i)
{

    plugin_exit ();
    exit (1);

}

#define SSO sizeof(so)
#define SIN sizeof(pi_init)
#define SEX sizeof(pi_exit)

int
main (argc, argv)
     int           argc;
     char        **argv;
{

    sigset_t      set;

    char          pi_init[80];
    char          pi_exit[80];

    char          so[80];
    char         *pi_name;
    int           pi_action;
    void         *handle;


    if (argv[1] == NULL || argv[2] == NULL)
	FATAL ("usage: loader {r|i} plugin");

    pi_action = *argv[1];
    pi_name = argv[2];

    if (pi_name == NULL)
	FATAL ("err: null plugin!");

    so[0] = '\0';

    pi_init[0] = '_';
    pi_exit[0] = '_';

    pi_init[1] = '\0';
    pi_exit[1] = '\0';

    strlcat (so, PLUGIN_PATH, SSO);
    strlcat (so, "/", SSO);
    strlcat (so, pi_name, SSO);
    strlcat (so, ".so", SSO);

    strlcat (pi_init, pi_name, SIN);
    strlcat (pi_exit, pi_name, SEX);

    strlcat (pi_init, "_init", SIN);
    strlcat (pi_exit, "_exit", SEX);

    // PUTS ("loading %s\n", so);

    handle = dlopen (so, RTLD_NOW);

    if (!handle)
	FATAL (dlerror ());

#if defined(__OpenBSD__)
    plugin_init = dlsym (handle, pi_init);
    plugin_exit = dlsym (handle, pi_exit);
    plugin_name = dlsym (handle, pi_name);

    plugin_name   = dlsym (handle, "_pi_name");
    plugin_string = dlsym (handle, "_pi_string");
    plugin_traname= dlsym (handle, "_pi_tarname");
    plugin_version= dlsym (handle, "_pi_version");
    plugin_bugrep = dlsym (handle, "_pi_bugrep");
    plugin_info   = dlsym (handle, "_pi_info");

#else
    plugin_init = dlsym (handle, pi_init + 1);
    plugin_exit = dlsym (handle, pi_exit + 1);

    plugin_name   = dlsym (handle, "pi_name");
    plugin_string = dlsym (handle, "pi_string");
    plugin_traname= dlsym (handle, "pi_tarname");
    plugin_version= dlsym (handle, "pi_version");
    plugin_bugrep = dlsym (handle, "pi_bugrep");
    plugin_info   = dlsym (handle, "pi_info"); 
#endif

    switch (pi_action) {
     case 'r':
	 PI_SIGNAL_INIT (&set);
	 signal (SIGUSR1, handler_usr1);
	 plugin_init ();
	 pause ();
	 break;
     case 'i':
	 printf("%s %s\t%s\n",plugin_name,plugin_version,plugin_bugrep);
         printf("        \t%s\n",plugin_info);
	 break;
     default:
	 FATAL ("usage: loader {r|i} plugin");
	 break;
    }
}
