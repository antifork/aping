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
#include "aping.h"
#include "global.h"

extern char *icmp_type_str[64];
extern char *icmp_code_str[64 * 64];

int true = 1;

/*
 * icmp header lenght ( base | x<<8 )
 * 
 * x:1   -> base + sizeof(data) x:2   -> base + sizeof(ip_header) ( <= 60 bytes
 * ) x:4   -> base + 4*n
 * 
 */

#define LEN(base,type) (base|type<<8)

int icmphdr_vector[32] = {LEN(8, 1), 0, 0, LEN(8, 2), LEN(8, 2), LEN(8, 2), 0, 0,
	LEN(8, 1), LEN(8, 4), 8, LEN(8, 2), LEN(8, 2), 20, 20, 8, 8, 12, 12
};


int
sizeof_icmp(int t)
{
	int ret;
	int tmp;

	tmp = icmphdr_vector[t & 0x1f];

	ret = tmp & 0xff;

	switch (tmp >> 8) {
	case 1:
		ret += sizeof(struct timeval) + strlen(pattern);
		break;
	case 2:
		ret += (options.opt_rroute ? 60 : 20) + (64 >> 3);	/* 64 bits of original
									 * datagram */
		break;
	case 4:
		break;
	}
	return ret;
}


void
load_layers(char *buff, packet * pkt)
{
	int off_ip;

	off_ip = (options.opt_rroute ? 15 : 5) << 2;

	pkt->ip = (struct ip *) ((void *) buff);
	pkt->icmp = (struct icmp *) ((void *) buff + off_ip);

	pkt->data = (char *) ((void *) buff + off_ip + 8);

	pkt->icmp_tstamp_tval = (struct timeval *) ((void *) buff + off_ip + 8);
	pkt->icmp_tstamp_data = (char *) ((void *) buff + off_ip + 8 + sizeof(struct timeval));
}


void
load_ip(struct ip * ip)
{
	char *ip_opt;

	ip->ip_v = 4;
	ip->ip_hl = (options.opt_rroute ? 15 : 5);
	ip->ip_tos = (options.ip_tos ? ip_tos : 0);

	ip->ip_len = FIX((ip->ip_hl << 2) + sizeof_icmp(icmp_type));
	ip->ip_id = htons(ip_id);

	if (options.dfrag)
		ip->ip_off = FIX(IP_DF);
	else
		ip->ip_off = FIX(0);

	ip->ip_ttl = (options.ip_ttl ? ip_ttl : 255);
	ip->ip_p = IPPROTO_ICMP;/* Transport protocol */

	ip->ip_src.s_addr = ip_src;
	ip->ip_dst.s_addr = ip_dst;

	if (options.opt_rroute) {
		ip_opt = (char *) ip + (5 << 2);

		ip_opt[IPOPT_OPTVAL] = IPOPT_RR;
		ip_opt[IPOPT_OLEN] = 3 + 4 * NROUTES;
		ip_opt[IPOPT_OFFSET] = IPOPT_MINOFF;

	}
	ip->ip_sum = 0;
	ip->ip_sum = chksum((u_short *) ip, ip->ip_hl << 2);

}


void
sender(argv)
	char **argv;
{

	struct timeval timenow;
	struct sockaddr_in saddr;
	packet pkt;
	int sfd;

	DEBUG("start\n");

	pthread_sigset_block(4, SIGTSTP, SIGINT, SIGQUIT, SIGALRM);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	gettimeofday(&timenow, NULL);

	last_sent.ts_sec = timenow.tv_sec;
	last_sent.ts_usec = timenow.tv_usec - tau * 1000;

	/* set icmp_id */

	memset(buffer, 0, MAX_PACSIZE);
	memset(&saddr, 0, sizeof(saddr));

	/* raw socket */

	if ((sfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1)
		FATAL("socket:%s", strerror(errno));

	/* ip header included */

#ifdef IP_HDRINCL
	if (setsockopt(sfd, IPPROTO_IP, IP_HDRINCL, &true, sizeof(true)) == -1)
		FATAL("IP_HDRINC: %s", strerror(errno));
#endif

	/* We set SO_BROADCAST whenever we need this. */

	if (ip_dst == (localnet | ~netmask)) {
		/* local broadcast */
		PUTS("%s: broadcast addr, SO_BROADCAST option is on.\n", ifname);
		setsockopt(sfd, SOL_SOCKET, SO_BROADCAST, (char *) &true, sizeof(true));
	}
	/* SO_DONTROUTE ... */

	if (options.droute) {
		PUTS("%s: SO_DONTROUTE option is on.\n", ifname);
		setsockopt(sfd, SOL_SOCKET, SO_DONTROUTE, (char *) &true, sizeof(true));
	}
	if (options.dfrag)
		PUTS("%s: IP_DF don't frag option is on.\n", ifname);

	if (options.so_debug) {
		PUTS("%s: SO_DEBUG option is on.\n", ifname);
		setsockopt(sfd, SOL_SOCKET, SO_DEBUG, (char *) &true, sizeof(true));
	}
	/* ip_id */

	if (!ip_id)
		ip_id = 0xffff & rand();

	/* struct saddr */

	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = ip_dst;

	/* lineup layer */

	load_layers(buffer, &pkt);

	if (icmp_code_str[(icmp_type << 6) + icmp_code] != NULL)
		PUTS("PING %s (%s): icmp=%ld(%s) code=%ld(%s)\n", host_dst, safe_inet_ntoa(ip_dst), icmp_type, icmp_type_str[icmp_type], icmp_code, icmp_code_str[(icmp_type << 6) + icmp_code]);
	else
		PUTS("PING %s (%s): icmp=%ld(%s)\n", host_dst, safe_inet_ntoa(ip_dst), icmp_type, icmp_type_str[icmp_type]);

	while (!count || (n_sent < count)) {

		/* load ip header */

		load_ip((struct ip *) (pkt.ip));

		/* load icmp header */

		if (icmp_loader_vector[icmp_type] != NULL)
			(*icmp_loader_vector[icmp_type]) (&pkt, argv);


		/* marking system timestamp for no-echo packets */

		gettimeofday(&timenow, NULL);

		last_sent.ts_sec = timenow.tv_sec;
		last_sent.ts_usec = timenow.tv_usec;

		switch (atomic_sendto(sfd, buffer, UNFIX(pkt.ip->ip_len), (struct sockaddr *) & saddr)) {
		case 0:
			PUTS("sendto(): 0 byte sent.\n");
			break;
		case -1:
			FATAL("sendto(): %s\n", strerror(errno));
		}

		n_sent++;
		print_icon(0);

		/* changes */

		if (options.ip_ramp)
			ip_ttl++;

		if (options.ip_id_incr || !options.ip_id)
			ip_id++;
		else if (options.ip_id_rand)
			ip_id = rand();

		/* rating.. */

		usleep(tau * 1000);

		while (n_pause)
			usleep(1000);
	}

	pthread_exit(NULL);

}
