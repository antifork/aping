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
#include "macro.h"

int
get_first_hop (target, source, ifname)
    long target;
    long *source;
    char *ifname;
{
    struct sockaddr_in saddr;
    int    sd;

    sd = 0;
    memset (&saddr, 0, sizeof (struct sockaddr_in));

    saddr.sin_family      = AF_INET;
    saddr.sin_addr.s_addr = target;

    saddr.sin_port = htons (1024 | rand ());

    if ((sd = socket (AF_INET, SOCK_DGRAM, 0)) == -1)
	FATAL (strerror (errno));

    if (!(ntohl (target) & 0xff) || (ntohl (target) & 0xff) == 0xff)
	{
	    /* broadcast */
	    int           true = 1;

	    setsockopt (sd, SOL_SOCKET, SO_BROADCAST, (char *) &true, sizeof (true));
	}


    if (connect (sd, (struct sockaddr *) &saddr, sizeof (struct sockaddr_in)) == -1)
	FATAL (strerror (errno));

    memset (&saddr, 0, sizeof (struct sockaddr_in));

    {
	int socksize = 0;
	socksize = sizeof (struct sockaddr);

	if (getsockname (sd, (struct sockaddr *) &saddr, &socksize) == -1)
	    FATAL (strerror (errno));
      
	*source = saddr.sin_addr.s_addr;
    }

    close (sd);

    {

	char          buffer[10240];

#ifdef __linux__ 
	char         *void_alias = NULL;
#endif
	u_long        ipif = 0;
	struct ifreq *ifr,
	             *iflast;
	struct ifconf ifc;
	struct sockaddr_in *local;

	ipif = saddr.sin_addr.s_addr;

	memset (buffer, 0, 10240);

	/* dummy dgram socket for ioctl */

	if ((sd = socket (AF_INET, SOCK_DGRAM, 0)) == -1)
	    FATAL (strerror (errno));

	ifc.ifc_len = sizeof (buffer);
	ifc.ifc_buf = buffer;

	/* getting ifs */

	if (ioctl (sd, SIOCGIFCONF, &ifc) < 0)
	    FATAL (strerror (errno));

	close (sd);

	/* line_up ifreq structure */

	ifr    = (struct ifreq *) buffer;
	iflast = (struct ifreq *) ((char *) buffer + ifc.ifc_len);

#if !defined (__linux__) && !defined(__sun__) 
	for (; ifr < iflast; (char *) ifr += sizeof (ifr->ifr_name) + ifr->ifr_addr.sa_len)
#else
	for (; ifr < iflast; (char *) ifr += sizeof (ifr->ifr_name) + sizeof (struct sockaddr_in))
#endif
	    {
		if (*(char *) ifr)
		    {
			local = (struct sockaddr_in *) &ifr->ifr_addr;
			if (ipif == local->sin_addr.s_addr)
			    {
				strncpy (ifname, ifr->ifr_name, 16);
/* alias */
#ifdef __linux__ 
				if ((void_alias = strchr (ifname, ':')) != NULL)
				    *void_alias = '\0';
#endif
				return 0;
			    }
		    }
	    }
	FATAL ("wasn't able to guess the ifname");

    }
    return -1;
}

long
gethostbyif(char *ifname)
{

        char            buffer[10240];
        int             sd;
        struct ifreq   *ifr, *iflast, ifreq_io;
        struct ifconf   ifc;
        struct sockaddr_in *paddr;
       

        memset(buffer, 0, 10240);
       
        /* dummy dgram socket for ioctl */
       
        if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
		FATAL (strerror (errno));
       
        ifc.ifc_len = sizeof(buffer);
        ifc.ifc_buf = buffer;
       
        /* getting ifs: this fills ifconf structure. */
       
        if (ioctl(sd, SIOCGIFCONF, &ifc) < 0) 
		FATAL (strerror (errno));

        close(sd);
       
        /* line_up ifreq structure */
       
        ifr = (struct ifreq *) buffer;
        iflast = (struct ifreq *) ((char *) buffer + ifc.ifc_len);
       
        if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
		FATAL (strerror (errno));

#if !defined(__linux__) && !defined(__sun__) 
        for (; ifr < iflast; (char *) ifr += sizeof(ifr->ifr_name) + ifr->ifr_addr.sa_len)
#else  
        for (; ifr < iflast; (char *) ifr += sizeof(ifr->ifr_name) + sizeof(struct sockaddr_in))
#endif 
        {
           if ( *(char *)ifr !=0 ) {

                   paddr = (struct sockaddr_in *) & ifr->ifr_addr;

		   memcpy(&ifreq_io, ifr, sizeof (ifr->ifr_name)+ sizeof(struct sockaddr_in)); 

		   if ( ioctl(sd, SIOCGIFFLAGS, &ifreq_io) < 0)
			FATAL("SIOCGIFFLAGS: %s",strerror(errno));

		   if ( ifreq_io.ifr_flags & IFF_UP )
		   	{
				if ( ifr->ifr_addr.sa_family == AF_INET)
					{
					if ( !strcmp (ifr->ifr_name,ifname) )
						return paddr->sin_addr.s_addr;
					
					}	

			}
                }
        }
       
        close(sd);

        FATAL ("device %s not found",ifname);

        return 0;
}      

