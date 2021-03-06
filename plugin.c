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
#include "typedef.h"
#include "prototype.h"
#include "macro.h"
#include "global.h"
#include "version.h"

static
const char cvsid[] = "$Id$";

void
plugin_init(char *name)
{
	char loader[80];
	char *arg[4];

	int pid;

	loader[0] = '\0';

	arg[0] = "loader";
	arg[1] = "r";
	arg[2] = name;
	arg[3] = NULL;

	strlcat(loader, PLUGIN_PATH, sizeof(loader));
	strlcat(loader, "/", sizeof(loader));
	strlcat(loader, "loader", sizeof(loader));

	switch (pid = fork()) {
	case -1:
		fatal("fork() error");
		break;
	case 0:
		/* plugin child */

		if (execve(loader, arg, NULL) == -1)
			fatal("loader error");

		break;
	default:
		if (pd_pindex < MAX_CHILD)
			pd_plugin[pd_pindex++] = pid;
		else {
			kill(pid, SIGUSR1);
			fatal("too many child");
		}
		break;
	}

}

void
plugin_ls()
{
	char loader[80];
	char *arg[4];

	struct dirent *dp;
	DIR *dfd;

	int pid;
	int status;

	loader[0] = '\0';

	arg[0] = "loader";
	arg[1] = "i";
	arg[2] = NULL;
	arg[3] = NULL;

	strlcat(loader, PLUGIN_PATH, sizeof(loader));
	strlcat(loader, "/", sizeof(loader));
	strlcat(loader, "loader", sizeof(loader));


	PUTS("plugins available:\n");

	dfd = opendir(PLUGIN_PATH);

	if (dfd == NULL)
		fatal("%s: directory doesn't exist. Install the package first", PLUGIN_PATH);

	while ((dp = readdir(dfd)) != NULL) {
		if (strcmp(".", dp->d_name) && strcmp("..", dp->d_name) && strstr(dp->d_name, ".so") != NULL) {

			switch (pid = fork()) {
			case -1:
				fatal("fork() error");
				break;
			case 0:
				/* plugin child */

				arg[2] = strdup(dp->d_name);
				*strstr(arg[2], ".so") = '\0';

				if (execve(loader, arg, NULL) == -1)
					fatal("loader error");

				break;
			default:
				waitpid(pid, &status, 0);
				break;

			}

		}
	}

	exit(0);
}
