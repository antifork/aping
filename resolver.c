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

#include "typedef.h"
#include "global.h"
#include "prototype.h"

#ifdef LRU
#error "LRU already defined"
#else
/* LRU must be a power of 2, and cannot be longer than 65536 elements. */
#define LRU  64
#endif

#define STA_MASK  0x0f 
#define STA_SIZE  STA_MASK+1
 
#define HASHMASK (\
LRU >> 1  | LRU >> 2  | LRU >> 3  |\
LRU >> 4  | LRU >> 5  | LRU >> 6  |\
LRU >> 7  | LRU >> 8  | LRU >> 9  |\
LRU >> 10 | LRU >> 11 | LRU >> 12 |\
LRU >> 13 | LRU >> 14 | LRU >> 15 )

/* table */

typedef struct _lru_
{
    unsigned int  yday;		/* day of the year */
    unsigned long addr;
    char         *host;
}
lru;

static lru    hostbyname[LRU];
static lru    hostbyaddr[LRU];

#define FATAL(f,arg...) {\
                        fprintf(stderr,"%s:%d: ",__FILE__,__LINE__);\
                        fprintf(stderr,f,## arg);\
                        fputs (". exit-forced\n",stderr);\
                        exit(1);\
                        }

#define FREE(a) { if ( a != NULL ) free(a); }

/* private fuctions */

/* gethostbyname utils */

static unsigned long
search_hostbyname (const char *host)
{
    register int  i;
    register long ret;

    ret = -1;
    i   = hash (host, strlen (host)) & HASHMASK ;

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
    register int  i;

    i = hash (h, strlen (h)) & HASHMASK ;

    FREE (hostbyname[i].host);

    hostbyname[i].host = strdup (h);
    hostbyname[i].addr = addr;

    return;
}

/* gethostbyaddr utils */

static char  *
search_hostbyaddr (const unsigned long addr)
{
    register int  i;
    char  	 *ret;

    ret = NULL;

    if (!addr) return ret;

    i = hash ((char *) &addr, sizeof (long int)) & HASHMASK;

    if (addr == hostbyaddr[i].addr)
	{
	    ret = hostbyaddr[i].host;
	}

    return ret;
}

static void
insert_hostbyaddr (const char *h, const unsigned long addr)
{
    register int  i;

    i = hash ((char *) &addr, 4) & HASHMASK;

    FREE (hostbyaddr[i].host);

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

    long          ret;

    if (host)
	{

	    if ((ret = search_hostbyname (host)) != -1)
	    /* hit */
		{
		    return ret;
		}

	    /* fail */

	    if ((addr.s_addr = inet_addr (host)) == -1)
		{
		    if (!(host_ent = gethostbyname (host)))
			FATAL ("gethostbyname_lru err:%s", strerror (errno));

		    bcopy (host_ent->h_addr, (char *) &addr.s_addr, host_ent->h_length);
		}

	    insert_hostbyname (host, addr.s_addr);
	    return addr.s_addr;

	}
    else
	FATAL ("gethostbyname_lru err: %s pointer", (char *) NULL);

    return 0; /* unreachable */

}


#define BUFF(x) x[i & STA_MASK]


char *
gethostbyaddr_lru (unsigned long addr)
{
    static int       i;

    static char     *ret[STA_SIZE];
           char     *tmp;

    struct hostent  *hostname;

    i++;

    FREE (BUFF(ret));

    if (addr == 0)
	return "0.0.0.0";
    
    if ( ! options.numeric )
	{

	    if ( (tmp=search_hostbyaddr (addr)) != NULL )
/* hit */
		{
		    BUFF(ret) = strdup (tmp);
		    return BUFF(ret);
		}
/* fail */
	    if ((hostname = gethostbyaddr ((char *) &addr, 4, AF_INET)) != NULL)
		BUFF(ret) = strdup (hostname->h_name);
	    else
		BUFF(ret) = strdup (inet_ntoa (*(struct in_addr *) &addr));

	}
    else
	BUFF(ret) = strdup (inet_ntoa (*(struct in_addr *) &addr));


    if (ret && addr)
	insert_hostbyaddr (BUFF(ret), addr);
    else
	FATAL ("gethostbyaddr_lru err:%s", strerror (errno));

    return BUFF(ret);
}


char *
multi_inet_ntoa (long address)
{

    static int    i;
    static char  *ret[STA_SIZE];

    i++;

    FREE(BUFF(ret));

    BUFF(ret) = strdup ( inet_ntoa (*(struct in_addr *) &address) );

    return (char *) BUFF(ret);

}

