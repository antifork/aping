/* $Id$ */
/*
 * $awgn: ns-cache.c,v 1.4 2002/09/30 17:37:54 awgn Exp $
 *
 * Copyright (c) 2002 Nicola Bonelli <bonelli@blackhats.it>
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

/*
 *  Headers
 */

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <time.h>
#include <assert.h>

int allow_reverse;

/*
 *  extern variables:
 *  -----------------
 *
 *  allow_reverse:	switch that controls gethostbyaddr_cache() query.
 *			In case of allow_reverse=0 a bare inet_ntoa() is
 *			performed. Hence -n option is usually used to
 *			show numerical addresses instead of trying to
 *			determine symbolic host, allow_reverse should be set
 *			to zero when -n is given; 1 otherwise.
 *
 *  config.h symbols:
 *  -----------------
 *
 *  __USE_NSHACK:	controls whether to use the alarm()/longjmp() hack or not.
 *
 *  __USE_PTHREAD:	controls whether this module is designed to join a
 *			pthread	project or not. In case of pthread programming,
 *			in order to use	__USE_NSHACK properly all threads must
 *			block SIGALRM.
 *
 * BUG: In some implementations of libc_r, it is not possible to use
 * __USE_HACK feature. OpenBSD up to release 3.1 is affected by this bug (feature?).
 *
 */

#define __USE_PTHREAD	1	/* aping is a phtread project */

#if defined(__USE_PTHREAD)
#include <pthread.h>
#endif

#define DNS_TIMEOUT	10	/* gethostbyname() timeout */
#define REV_TIMEOUT	5	/* gethostbyaddr() timeout */
#define EXPIRE_DAY	2
#define HASH_DEPTH	12	/* 2^HASH_DEPTH table entries */
#define HASH_MASK	((1<<HASH_DEPTH)-1)
#define HASH_TABSIZE    (1<<HASH_DEPTH)
#define SAFE_BUFFERS    (1<<8)
#define SAFE_MASK       (SAFE_BUFFERS-1)

typedef unsigned long nbo;

struct entry {
	nbo addr;
	char *host;
	short yday;
};

static char *sf[SAFE_BUFFERS];	/* used to return __safe_buffer */

static struct entry hostbyname[HASH_TABSIZE];
static struct entry hostbyaddr[HASH_TABSIZE];

#if defined(__USE_NSHACK)
static jmp_buf gethost_jmp;
#endif

/*
 * Private Functions
 *
 */

#if defined(__USE_NSHACK)
static void
__abort_query(int s)
{
	longjmp(gethost_jmp, 1);
}
#endif

static void
__prolog_signal()
{
#if defined (__USE_PTHREAD) && defined(__USE_NSHACK)
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGALRM);
	pthread_sigmask(SIG_UNBLOCK, &set, NULL);
#endif
}

static void
__epilog_signal()
{
#if defined (__USE_PTHREAD) && defined(__USE_NSHACK)
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGALRM);
	pthread_sigmask(SIG_BLOCK, &set, NULL);
#endif
}

/*
 ***
 *
 * Fowler/Noll/Vo hash
 *
 * The basis of this hash algorithm was taken from an idea sent
 * as reviewer comments to the IEEE POSIX P1003.2 committee by:
 *
 *      Phong Vo (http://www.research.att.com/info/kpv/)
 *      Glenn Fowler (http://www.research.att.com/~gsf/)
 *
 * In a subsequent ballot round:
 *
 *      Landon Curt Noll (http://www.isthe.com/chongo/)
 *
 * improved on their algorithm.  Some people tried this hash
 * and found that it worked rather well.  In an EMail message
 * to Landon, they named it the ``Fowler/Noll/Vo'' or FNV hash.
 *
 * FNV hashes are designed to be fast while maintaining a low
 * collision rate. The FNV speed allows one to quickly hash lots
 * of data while maintaining a reasonable collision rate.  See:
 *
 *      http://www.isthe.com/chongo/tech/comp/fnv/index.html
 *
 * for more details as well as other forms of the FNV hash.
 ***
 *
 * Please do not copyright this code.  This code is in the public domain.
 *
 * LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * By:
 *      chongo <Landon Curt Noll> /\oo/\
 *      http://www.isthe.com/chongo/
 *
 * Share and Enjoy!     :-)
 */

#define FNV32_prime 16777619UL
#define FNV32_init  2166136261UL

static unsigned long
__fnv(const char *p, int s)
{
	unsigned long h = FNV32_init;
	int i = 0;

	for (; i < s; i++) {
		h = h * FNV32_prime;
		h = h ^ p[i];
	}

	return h;
}

static nbo
__cache_hostbyname(const char *host)
{
	time_t t;
	int i;

	if (host == NULL)
		return -1;
	i = (__fnv(host, strlen(host)) & HASH_MASK);

	t = time(NULL);
	if ((hostbyname[i].yday + EXPIRE_DAY) <= gmtime(&t)->tm_yday)
		return -1;	/* expired */
	if (hostbyname[i].host == NULL)
		return -1;
	if (!strcmp(host, hostbyname[i].host))
		return (hostbyname[i].addr);

	return -1;
}

static char *
__cache_hostbyaddr(const nbo address)
{
	time_t t;
	int i;

	if (address == 0)
		return NULL;
	i = __fnv((char *) &address, sizeof(nbo)) & HASH_MASK;
	t = time(NULL);

	if ((hostbyaddr[i].yday + EXPIRE_DAY) <= gmtime(&t)->tm_yday)
		return NULL;	/* expired */

	if (hostbyaddr[i].addr == address) {
		return (hostbyaddr[i].host);
	}
	return NULL;
}

static void
__insert_hostbyname(const char *h, const nbo addr)
{
	time_t t;
	int i;

	assert(h != NULL);
	i = __fnv(h, strlen(h)) & HASH_MASK;
	t = time(NULL);

	free(hostbyname[i].host);
	hostbyname[i].host = strdup(h);
	hostbyname[i].addr = addr;
	hostbyname[i].yday = (short) (gmtime(&t)->tm_yday);
	return;
}

static void
__insert_hostbyaddr(const char *h, const nbo addr)
{
	time_t t;
	int i;

	assert(h != NULL);
	i = __fnv((char *) &addr, sizeof(nbo)) & HASH_MASK;
	t = time(NULL);

	free(hostbyaddr[i].host);
	hostbyaddr[i].host = strdup(h);
	hostbyaddr[i].addr = addr;
	hostbyaddr[i].yday = (short) (gmtime(&t)->tm_yday);
	return;
}

char *
__safe_buffer(char *new)
{
	static int i;
	i++;

	free(sf[i & SAFE_MASK]);
	sf[i & SAFE_MASK] = strdup(new);
	return (sf[i & SAFE_MASK]);
}

/*
 *  Public Functions
 *
 */


nbo
gethostbyname_cache(const char *host)
{
	struct hostent *host_ent;
	struct in_addr addr;
	nbo ret;

	__prolog_signal();

	if (host == NULL) {
		fprintf(stderr, "gethostbyname(%s) error: NULL ptr?\n", host);
		__epilog_signal();
		return -1;
	}
	if ((ret = __cache_hostbyname(host)) != -1) {
		/* HIT */
		__epilog_signal();
		return ret;
	}
	if ((addr.s_addr = inet_addr(host)) == -1) {
		/* host is not a ipv4 dot form */
		/* void DNS timeout */
#if defined(__USE_NSHACK)
		if (!setjmp(gethost_jmp)) {
			signal(SIGALRM, __abort_query);
			alarm(DNS_TIMEOUT);
#endif
			host_ent = gethostbyname(host);
#if defined(__USE_NSHACK)
			alarm(0);
#endif
			if (host_ent == NULL) {
				fprintf(stderr, "gethostbyname(%s) error: host not found\n", host);
				__epilog_signal();
				return -1;
			}
			bcopy(host_ent->h_addr, (char *) &addr.s_addr, host_ent->h_length);
#if defined(__USE_NSHACK)
		} else {
			fprintf(stderr, "gethostbyname(%s) error: timeout\n", host);
			__epilog_signal();
			return -1;
		}
#endif
	}
	assert(host != NULL);
	__insert_hostbyname(host, addr.s_addr);

	__epilog_signal();
	return addr.s_addr;
}

char *
gethostbyaddr_cache(const nbo addr)
{
	struct hostent *hostname;
	char *ret;

	__prolog_signal();
	if (addr == 0) {
		__epilog_signal();
		return __safe_buffer("0.0.0.0");
	}
	if (allow_reverse != 0) {
		if ((ret = __cache_hostbyaddr(addr)) != NULL) {
			__epilog_signal();
			return __safe_buffer(ret);
		}
		/* fail */
		/* void DNS timeout */

#if defined(__USE_NSHACK)
		if (!setjmp(gethost_jmp)) {
			signal(SIGALRM, __abort_query);
			alarm(REV_TIMEOUT);
#endif
			hostname = gethostbyaddr((char *) &addr, 4, AF_INET);
#if defined(__USE_NSHACK)
			alarm(0);
#endif
			if (hostname != NULL)
				ret = hostname->h_name;
			else
				ret = inet_ntoa(*(struct in_addr *) & addr);
#if defined(__USE_NSHACK)
		} else
			ret = inet_ntoa(*(struct in_addr *) & addr);
#endif

	} else
		ret = inet_ntoa(*(struct in_addr *) & addr);

	assert(ret != NULL);
	__insert_hostbyaddr(ret, addr);

	__epilog_signal();
	return __safe_buffer(ret);
}

char *
safe_inet_ntoa(const nbo address)
{
	char *ret;
	ret = inet_ntoa(*(struct in_addr *) & address);
	return __safe_buffer(ret);
}
