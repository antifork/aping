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


#include "header.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

#include <pthread.h>

#include <dlfcn.h>

#include "typedef.h"
#include "prototype.h"

#include "macro.h"
#include "global.h"
#include "version.h"

long   (*plugin) (void);

void
plugin_init(char *name)
{
  char sy[80];
  char so[80]; 

  void *handle;

  if ( name == NULL )
	FATAL("err: null plugin!");

  so[0]='\0';

  sy[0]='_';
  sy[1]='\0';

  strcat (so,PLUGIN_PATH);
  strcat (so,"/");
  strncat(so,name,80-4-sizeof(PLUGIN_PATH));
  strcat (so,".so");

  strncat (sy,name,79-6);
  strcat  (sy,"_init");

  PUTS("loading %s\n",so);

  handle = dlopen (so, RTLD_NOW);

  if (!handle) 
  	FATAL("dlerror()"); 
   
  #if defined(__OpenBSD__)
   plugin = dlsym(handle, sy);
  #else                             
   plugin = dlsym(handle, sy+1);
  #endif

  pthread_create (&pd_plugin[pd_pindex++], NULL, (void *)plugin, NULL);

  
}

void
plugin_ls()
{
  struct dirent *dp; 
  DIR *dfd;

  PUTS("plugins available:\n");

  dfd = opendir(PLUGIN_PATH);

  if ( dfd == NULL )
	FATAL("%s: directory doesn't exist. Install the package first", PLUGIN_PATH);

  while ( (dp = readdir (dfd)) != NULL)
	{

	if ( strcmp(".",dp->d_name ) && strcmp("..",dp->d_name ) ) 
		{
		PUTS("%s ",dp->d_name);
		}
	}

  PUTS("\n");

  exit(1);

}
