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


/* standard header */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>

#include <netdb.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>

#include "typedef.h"
#include "global.h"
#include "prototype.h"
#include "macro.h"

#ifdef LRU
#error "LRU already defined"
#else
/* LRU must be a power of 2, and cannot be longer than 65536 elements. */
#define LRU  64
#endif

#define STA_MASK  	0x0f
#define STA_SIZE  	STA_MASK+1
#define DNS_TIMEOUT     10	/* gethostbyname timeout */
#define REV_TIMEOUT	1	/* gethostbyaddr timeout */

#define HASHMASK (\
LRU >> 1  | LRU >> 2  | LRU >> 3  |\
LRU >> 4  | LRU >> 5  | LRU >> 6  |\
LRU >> 7  | LRU >> 8  | LRU >> 9  |\
LRU >> 10 | LRU >> 11 | LRU >> 12 |\
LRU >> 13 | LRU >> 14 | LRU >> 15 )

/* table */

typedef struct _lru_ {
    unsigned int yday;		/* day of the year */
    unsigned long addr;
    char *host;
} lru;

static lru hostbyname[LRU];
static lru hostbyaddr[LRU];

#ifdef NS_HACK
static jmp_buf gethost_jmp;
#endif

/* private fuctions */

/* gethostbyname utils */

#ifdef NS_HACK
static void
abort_query (int s)
{
    longjmp (gethost_jmp, 1);
}
#endif

static void
prolog_signal()
{
#ifdef NS_HACK
    sigset_t set;
    sigemptyset (&set);
    sigaddset (&set, SIGALRM);
    pthread_sigmask (SIG_UNBLOCK, &set, NULL);
#endif
}

static void
epilog_signal()
{
#ifdef NS_HACK
    sigset_t set;
    sigemptyset (&set);
    sigaddset (&set, SIGALRM);   
    pthread_sigmask (SIG_BLOCK, &set, NULL);
#endif
}

static unsigned long
search_hostbyname (const char *host)
{
    register int i;
    register long ret;

    ret = -1;
    i = hash (host, strlen (host)) & HASHMASK;

    /* null */

    if (hostbyname[i].host == NULL)
	return ret;
    if (!strcmp (host, hostbyname[i].host))
	ret = hostbyname[i].addr;

    return ret;
}

static void
insert_hostbyname (const char *h, const unsigned long addr)
{
    register int i;

    i = hash (h, strlen (h)) & HASHMASK;
    free (hostbyname[i].host);

    hostbyname[i].host = strdup (h);
    hostbyname[i].addr = addr;

    return;
}

/* gethostbyaddr utils */

static char *
search_hostbyaddr (const unsigned long addr)
{
    register int i;
    char *ret;

    ret = NULL;

    if (!addr)
	return ret;

    i = hash ((char *) &addr, sizeof (long int)) & HASHMASK;

    if (addr == hostbyaddr[i].addr) {
	ret = hostbyaddr[i].host;
    }

    return ret;
}

static void
insert_hostbyaddr (const char *h, const unsigned long addr)
{
    register int i;

    i = hash ((char *) &addr, 4) & HASHMASK;
    free (hostbyaddr[i].host);

    hostbyaddr[i].host = strdup (h);
    hostbyaddr[i].addr = addr;

    return;
}

/*** public function ***/

long
gethostbyname_lru (const char *host)
{
    struct in_addr addr;
    struct hostent *host_ent;
    long ret;

    prolog_signal();

    if (host) {

	if ((ret = search_hostbyname (host)) != -1)
	    /* hit */
	{
	 epilog_signal();
         return ret;
	}
	/* fail */

	if ((addr.s_addr = inet_addr (host)) == -1) {

	    /* void DNS timeout */
#ifdef NS_HACK
	    if (!setjmp (gethost_jmp)) {
		signal (SIGALRM, abort_query);
		alarm (DNS_TIMEOUT);
#endif
		host_ent = gethostbyname (host);
#ifdef NS_HACK
		alarm (0);
#endif
		if (host_ent == NULL)
		    FATAL ("gethostbyname_lru(%s) err", host);
		bcopy (host_ent->h_addr, (char *) &addr.s_addr, host_ent->h_length);
#ifdef NS_HACK
	    }
	    else
		FATAL ("gethostbyname_lru(%s) err", host);
#endif
	}
	insert_hostbyname (host, addr.s_addr);
	epilog_signal();
	return addr.s_addr;

    }
    else
	FATAL ("gethostbyname_lru err: %s pointer", (char *) NULL);

    return 0;			/* unreachable */

}


#define BUFF(x) x[i & STA_MASK]


char *
gethostbyaddr_lru (unsigned long addr)
{
    struct hostent *hostname;
    static char *ret[STA_SIZE];
    static int i;
    char *tmp;

    prolog_signal();

    i++;

    free (BUFF (ret));
    if (addr == 0)
	{
	epilog_signal();
	return "0.0.0.0";
	}

    if (!options.numeric) {

	if ((tmp = search_hostbyaddr (addr)) != NULL)
/* hit */
	{
	    BUFF (ret) = strdup (tmp);
	    epilog_signal();
	    return BUFF (ret);
	}
/* fail */

	/* void DNS timeout */

#ifdef NS_HACK
	if (!setjmp (gethost_jmp)) {
	    signal (SIGALRM, abort_query);
	    alarm (REV_TIMEOUT);
#endif
	    hostname = gethostbyaddr ((char *) &addr, 4, AF_INET);
#ifdef NS_HACK
	    alarm (0);
#endif
	    if (hostname != NULL)
		BUFF (ret) = strdup (hostname->h_name);
	    else
		BUFF (ret) = strdup (inet_ntoa (*(struct in_addr *) &addr));
#ifdef NS_HACK
	}
	else
	    BUFF (ret) = strdup (inet_ntoa (*(struct in_addr *) &addr));
#endif

    }
    else
	BUFF (ret) = strdup (inet_ntoa (*(struct in_addr *) &addr));


    if (ret && addr)
	insert_hostbyaddr (BUFF (ret), addr);
    else
	FATAL ("gethostbyaddr_lru err:%s", strerror (errno));

    epilog_signal();
    return BUFF (ret);

}


char *
multi_inet_ntoa (long address)
{

    static int i;
    static char *ret[STA_SIZE];

    i++;
    free (BUFF (ret));

    BUFF (ret) = strdup (inet_ntoa (*(struct in_addr *) &address));

    return (char *) BUFF (ret);

}
