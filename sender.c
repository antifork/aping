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


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <signal.h>

#include <pcap.h>
#include <pthread.h>

#include <string.h>
#include <errno.h>

#include <sys/time.h>
#include <time.h>

#include <sys/socket.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>

#include "typedef.h"
#include "macro.h"

#include "aping.h"
#include "prototype.h"
#include "global.h"


extern char  *icmp_type_str[24];

int           true = 1;

/* 
 * icmp header lenght ( base | x<<8 )
 *
 * x:1   -> base + sizeof(data)
 * x:2   -> base + sizeof(ip_header) ( <= 60 bytes )
 * x:4   -> base + 4*n
 *
 */

#define LEN(base,type) (base|type<<8)

int           icmphdr_vector[32] = { LEN (8, 1), 0, 0, LEN (8, 2), LEN (8, 2), LEN (8, 2), 0, 0,
    LEN (8, 1), LEN (8, 4), 8, LEN (8, 2), LEN (8, 2), 20, 20, 8, 8, 0, 0
};


void          (*icmp_load_vector[32]) (packet *, char **) =
{
load_echo_reply,
	NULL,
	NULL,
	load_destination_unreachable,
	load_source_quench,
	load_redirect,
	NULL,
	NULL,
	load_echo,
	load_router_advertisement,
	load_router_solicitation,
	load_time_exceeded, load_parameter_problem, load_timestamp, load_timestamp_reply, load_information_request, load_information_reply, load_address_mask_request, load_address_mask_reply};


int
sizeof_icmp (int t)
{
    int           ret;
    int           tmp;

    tmp = icmphdr_vector[t & 0x1f];

    ret = tmp & 0xff;

    switch (tmp >> 8)
	{
	 case 1:
	     ret += sizeof (struct timeval) + strlen (pattern);
	     break;
	 case 2:
	     ret += (options.opt_rroute ? 60 : 20) + (64 >> 3);	/* 64 bits of original datagram */
	     break;
	 case 4:
	     break;
	}
    return ret;
}


void
load_layers (char *buff, packet * pkt)
{
    int           off_ip;

    off_ip = (options.opt_rroute ? 15 : 5) << 2;

    pkt->ip = (struct ip *) ((void *) buff);
    pkt->icmp = (struct icmp *) ((void *) buff + off_ip);

    pkt->data = (char *) ((void *) buff + off_ip + 8);

    pkt->icmp_tstamp_tval = (struct timeval *) ((void *) buff + off_ip + 8);
    pkt->icmp_tstamp_data = (char *) ((void *) buff + off_ip + 8 + sizeof (struct timeval));
}


void
load_ip (struct ip *ip)
{

    ip->ip_v = 4;
    ip->ip_hl = (options.opt_rroute ? 15 : 5);
    ip->ip_tos = (options.ip_tos ? ip_tos : 0);

    ip->ip_len = FIX ((ip->ip_hl << 2) + sizeof_icmp (icmp_type));
    ip->ip_id = htons (ip_id);

    ip->ip_off = FIX (0);

    ip->ip_ttl = (options.ip_ttl ? ip_ttl : 255);
    ip->ip_p = IPPROTO_ICMP;	/* Transport protocol */

    ip->ip_sum = 0;

    ip->ip_src.s_addr = ip_src;
    ip->ip_dst.s_addr = ip_dst;

    ip->ip_sum = chksum ((u_short *) ip, ip->ip_hl << 2);

}


void
sender (argv)
     char        **argv;
{

    struct timeval timenow;

    struct sockaddr_in saddr;
    sigset_t      set;
    packet        pkt;
    int           sfd;

    sigemptyset (&set);

    sigaddset (&set, SIGTSTP);
    sigaddset (&set, SIGINT);
    sigaddset (&set, SIGQUIT);

    pthread_sigmask (SIG_BLOCK, &set, NULL);

    pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, NULL);


    /* set icmp_id */

    memset (buffer, 0, MAX_PACSIZE);
    memset (&saddr, 0, sizeof (saddr));

    /* raw socket */

    if ((sfd = socket (AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1)
	FATAL ("socket:%s", strerror (errno));

    /* ip header included */

#ifdef IP_HDRINCL
    if (setsockopt (sfd, IPPROTO_IP, IP_HDRINCL, &true, sizeof (true)) == -1)
	FATAL ("IP_HDRINC: %s", strerror (errno));
#endif

    /* We set SO_BROADCAST whenever we need this. */

    if (!(ntohl (ip_dst) & 0xff) || (ntohl (ip_dst) & 0xff) == 0xff)
	{
	    /* broadcast */
	    PUTS ("broadcast addr, setting SO_BROADCAST socket option.\n");
	    setsockopt (sfd, SOL_SOCKET, SO_BROADCAST, (char *) &true, sizeof (true));
	}


    /* ip_id */

    if (!ip_id)
	ip_id = 0xffff & rand ();

    /* struct saddr */

    saddr.sin_family      = AF_INET;
    saddr.sin_addr.s_addr = ip_dst;

    /* lineup layer */

    load_layers (buffer, &pkt);

    PUTS ("PING %s (%s): icmp=%ld(%s)\n", host_dst, multi_inet_ntoa (ip_dst), icmp_type, icmp_type_str[icmp_type]);


    while (!count || (n_sent < count))
	{

	/* load ip header */

	    load_ip ((struct ip *) (pkt.ip));

	/* load icmp header */

	    if (icmp_load_vector[icmp_type] != NULL)
		(*icmp_load_vector[icmp_type]) (&pkt, argv);


	/* marking system timestamp for no-echo packets */

	    gettimeofday (&timenow, NULL);

	    last_sent.ts_sec  = timenow.tv_sec;
	    last_sent.ts_usec = timenow.tv_usec;

	    switch (sendto(sfd, buffer, UNFIX (pkt.ip->ip_len), 0, (struct sockaddr *) &saddr, sizeof (struct sockaddr)))
		{
		 case 0:
		     PUTS ("sendto: 0 byte sent\n");
		     break;
		 case -1:
		     PUTS ("sendto: %s\n", strerror (errno));
		     break;
		}

	    n_sent++ ;

	    print_icon(0);

            /* rate */ 

            usleep (tau * 1000);
	
	    /* changes */

	    if (options.ip_ramp                      ) ip_ttl++;
	    if (options.ip_id_incr || !options.ip_id ) ip_id ++;
	    else 
            if (options.ip_id_rand                   ) ip_id = rand ();

	}

    pthread_exit (NULL);

}
