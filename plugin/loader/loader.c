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



long   (*plugin_init) (void);
long   (*plugin_exit) (void);

void
handler_usr1(int i)
{

  plugin_exit();
  exit(1);
 
}


int
main(argc, argv)
   int    argc;
   char **argv;
{
  
  sigset_t 	set;

  char 		pi_init[80];
  char          pi_exit[80];

  char 		so[80];
  char 		*name; 
  void 		*handle;

  name = argv[1];
 
  if ( name == NULL )
        FATAL("err: null plugin!");

  so[0]='\0';

  pi_init[0]='_';
  pi_exit[0]='_';

  pi_init[1]='\0';
  pi_exit[1]='\0';

  strcat (so,PLUGIN_PATH);
  strcat (so,"/");
  strncat(so,name,80-4-sizeof(PLUGIN_PATH));
  strcat (so,".so");

  strncat (pi_init,name,79-6);
  strncat (pi_exit,name,79-6);

  strcat  (pi_init,"_init");
  strcat  (pi_exit,"_exit");

  PUTS("loading %s\n",so);

  handle = dlopen (so, RTLD_NOW);

  if (!handle)
        FATAL(dlerror());

  #if defined(__OpenBSD__)
   plugin_init = dlsym(handle, pi_init);
   plugin_exit = dlsym(handle, pi_exit);
  #else
   plugin_init = dlsym(handle, pi_init+1);
   plugin_exit = dlsym(handle, pi_exit+1);
  #endif

   PI_SIGNAL_INIT(&set);

   signal (SIGUSR1, handler_usr1);

   plugin_init();  

}