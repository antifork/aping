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
#include "config.h"

static
const char cvsid[] = "$Id$";

int
get_first_hop(target, source, ifname)
	long target;
	long *source;
	char *ifname;
{
	char buffer[10240];
	struct sockaddr_in saddr;

#ifdef __linux__
	char *void_alias = NULL;
#endif
	struct ifreq *ifr, *iflast, ifreq_io;
	struct ifconf ifc;
	struct sockaddr_in *local;
	u_long ipif;
	int socksize;
	int true;
	int sd;

	sd = 0;
	true = 1;
	socksize = sizeof(struct sockaddr);

	memset(&saddr, 0, sizeof(struct sockaddr_in));
	memset(&ifreq_io, 0, sizeof(struct ifreq));

	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = target;
	saddr.sin_port = htons(1024 | rand());

	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		fatal(strerror(errno));

	setsockopt(sd, SOL_SOCKET, SO_BROADCAST, (char *) &true, sizeof(true));

	if (connect(sd, (struct sockaddr *) & saddr, sizeof(struct sockaddr_in)) == -1)
		fatal(strerror(errno));

	memset(&saddr, 0, sizeof(struct sockaddr_in));

	if (getsockname(sd, (struct sockaddr *) & saddr, &socksize) == -1)
		fatal(strerror(errno));

	*source = saddr.sin_addr.s_addr;
	close(sd);

	if (*source == target) {
		unsigned long loopback;

		loopback = (unsigned long) inet_addr("127.0.0.1");
		memcpy(&saddr.sin_addr, &loopback, sizeof(struct in_addr));
	}
	memset(buffer, 0, 10240);

	ipif = saddr.sin_addr.s_addr;
	/* dummy dgram socket for ioctl */

	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		fatal(strerror(errno));

	ifc.ifc_len = sizeof(buffer);
	ifc.ifc_buf = buffer;

	/* getting ifs */
	if (ioctl(sd, SIOCGIFCONF, &ifc) < 0)
		fatal(strerror(errno));

	/* line_up ifreq structure */

	ifr = (struct ifreq *) buffer;
	iflast = (struct ifreq *) ((char *) buffer + ifc.ifc_len);

#if defined (HAVE_SOCKADDR_SA_LEN)
	for (; ifr < iflast; (char *) ifr += sizeof(ifr->ifr_name) + ifr->ifr_addr.sa_len)
#else
	for (; ifr < iflast; (char *) ifr += sizeof(ifr->ifr_name) + sizeof(struct sockaddr_in))
#endif
	{

		strncpy(ifreq_io.ifr_name, ifr->ifr_name, 16);


		if (ioctl(sd, SIOCGIFFLAGS, &ifreq_io) < 0)
			fatal("SIOCGIFFLAGS: %s", strerror(errno));

		if ((ifreq_io.ifr_flags & IFF_UP) == 0)
			continue;

		local = (struct sockaddr_in *) & ifr->ifr_addr;

		if (ipif == local->sin_addr.s_addr) {
			strncpy(ifname, ifr->ifr_name, 16);
			/* alias */
#ifdef __linux__
			if ((void_alias = strchr(ifname, ':')) != NULL)
				*void_alias = '\0';
#endif
			close(sd);
			return 0;
		}
	}
	fatal("wasn't able to guess the ifname");

	return -1;
}

long
gethostbyif(char *ifname)
{
	char buffer[10240];
	int sd;
	struct ifreq *ifr, *iflast, ifreq_io;
	struct ifconf ifc;
	struct sockaddr_in *paddr;


	memset(buffer, 0, 10240);

	/* dummy dgram socket for ioctl */

	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		fatal(strerror(errno));

	ifc.ifc_len = sizeof(buffer);
	ifc.ifc_buf = buffer;

	/* getting ifs: this fills ifconf structure. */

	if (ioctl(sd, SIOCGIFCONF, &ifc) < 0)
		fatal(strerror(errno));

	close(sd);

	/* line_up ifreq structure */

	ifr = (struct ifreq *) buffer;
	iflast = (struct ifreq *) ((char *) buffer + ifc.ifc_len);

	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		fatal(strerror(errno));

#if defined(HAVE_SOCKADDR_SA_LEN)
	for (; ifr < iflast; (char *) ifr += sizeof(ifr->ifr_name) + ifr->ifr_addr.sa_len)
#else
	for (; ifr < iflast; (char *) ifr += sizeof(ifr->ifr_name) + sizeof(struct sockaddr_in))
#endif
	{
		if (*(char *) ifr == 0)
			continue;

		paddr = (struct sockaddr_in *) & ifr->ifr_addr;
		memcpy(&ifreq_io, ifr, sizeof(ifr->ifr_name) + sizeof(struct sockaddr_in));

		if (ioctl(sd, SIOCGIFFLAGS, &ifreq_io) < 0)
			fatal("SIOCGIFFLAGS: %s", strerror(errno));

		if ((ifreq_io.ifr_flags & IFF_UP) == 0)
			continue;

		if (ifr->ifr_addr.sa_family != AF_INET)
			continue;

		if (!strcmp(ifr->ifr_name, ifname)) {
			close(sd);	/* xenion */
			return paddr->sin_addr.s_addr;
		}
	}

	close(sd);

	fatal("device %s not found", ifname);
	return 0;
}
