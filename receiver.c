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
#include "aping.h"

#include "typedef.h"
#include "prototype.h"
#include "macro.h"


#include "hardware.h"
#include "global.h"
#include "def_icmp.h"

int icmp_parent[32] = { 8, -1, -1, -1, -1, -1, -1, -1, 0, -1, -1, -1, -1, 0, 13, 0, 15, 0, 17, -1, -1 };


struct pcap_pkthdr pcaphdr;
struct bpf_program fcode;
struct timeval *timestamp;
packet pkt;

#include "filter.h"

void
lineup_layers (char *buff, packet * pkt)
{

    pkt->dl = (u_char *) buff;
    pkt->ip = (struct ip *) ((void *) buff + offset_dl);
    pkt->icmp = (struct icmp *) ((void *) buff + offset_dl + (pkt->ip->ip_hl << 2));
    pkt->data = (char *) ((void *) buff + offset_dl + (pkt->ip->ip_hl << 2) + 8);
    pkt->icmp_tstamp_tval = (struct timeval *) ((void *) buff + offset_dl + (pkt->ip->ip_hl << 2) + 8);
    pkt->icmp_tstamp_data = (char *) ((void *) buff + offset_dl + (pkt->ip->ip_hl << 2) + 8 + sizeof (struct timeval));

}

unsigned short
ntohss (s)
     unsigned short s;
{
    unsigned short ret;

    ret = 0;

    if (endian_bug & 2) {
	ret = s;
    }
    else {
	ret |= (s >> 8);
	ret |= (s << 8);
    }

    return ret;
}


#ifdef __GNUC__
__inline
#endif
    void
agent_timestamp (packet * p)
{

    switch (ICMP_type (p)) {

     case ICMP_ECHOREPLY:

	 rtt.ms_int = DIFFTIME_int (timestamp->tv, TVAL_tv (p));
	 rtt.ms_frc = DIFFTIME_frc (timestamp->tv, TVAL_tv (p));

	 break;

     case ICMP_TSTAMPREPLY:	/* timestamp reply */

	 rtt.ms_int = (timestamp->tv_sec % (24 * 60 * 60)) * 1000 + timestamp->tv_usec / 1000 - ntohl (ICMP_otime (p));
	 rtt.ms_frc = 0;

	 curr_tstamp = ntohl (ICMP_rtime (p));

	 break;

     default:			/* stimated rtt */

	 rtt.ms_int = DIFFTIME_int (timestamp->tv, last_sent.ts);
	 rtt.ms_frc = DIFFTIME_frc (timestamp->tv, last_sent.ts);
	 break;

    }


    TIME_ADJUST (rtt.ms_int, rtt.ms_frc);

    rtt.ms_int = ABS (rtt.ms_int);

    rtt_max = MAX (rtt_max, rtt.ms_int + 1);
    rtt_min = MIN (rtt_min, rtt.ms_int);

    E (&rtt_mean, rtt.ms_int, 1);
    E (&rtt_sqre, rtt.ms_int, 2);

    jitter = rtt.ms_int - last_rtt.ms_int;


    return;
}


void
reset_ipid ()
{

    rand_ip_id = 0;
    ipid_failure = 0;
    slow_start = 1;

}


void
reset_stat ()
{
    out_burst = 0;
    max_burst = 0;
    rand_ip_id = 0;
    ipid_failure = 0;
    slow_start = 1;

    E (&mean_burst, 0, -1);
    lp_FIR (-1, 0);
}


#ifdef __GNUC__
__inline
#endif
    int
agent_ipid (packet * p)
{

    if (diff_id && !(diff_id & 0xff)) {
	/* wrong id-endian */

	ipid_failure++;

	switch (ipid_failure) {
	 case 1:
	     break;
	 case 2:
	 case 3:
	     PUTS ("\n!!!WARNING: wrong ip_id endianess!!!");
	     return -1;
	 case 4:
	     reset_stat ();
	     PUTS ("\n!!!REVERTING to correct endian!!! ");
	     SWITCH (endian_bug);
	     return -1;

	}
    }

    else
	ipid_failure = 0;

    return 0;
}


#ifdef __GNUC__
__inline
#endif
    void
agent_dyn_id (packet * p)
{

    curr_id = ntohss (IP_id (p));

    if (slow_start) {
	diff_id = 0;
	slow_start = 0;
    }
    else
	diff_id = curr_id - last_id;

    if (diff_id < 0)
	diff_id += (1 << 16) - 1;

    if (diff_id > 1 << 15)
	rand_ip_id = diff_id;

    return;
}


void
print_RR (char *opt)
{
    long *hop;
    long rr;

    int i;

    rr = hash (opt, 40);

    if (last_route != rr) {
	/* new RR */
	last_route = rr;

	hop = (long *) (opt + 3);	/* hop pointer */

	for (i = 1; i < MIN (9, (opt[IPOPT_OFFSET] >> 2)); i++) {
	    PUTS ("\rRR:  %s(%s)\n", gethostbyaddr_lru (hop[i - 1]), multi_inet_ntoa (hop[i - 1]));
	}

    }

}


void
process_pack (packet * p)
{
    char *saddr;
    char *daddr;

    saddr = gethostbyaddr_lru (IP_src (p));
    daddr = gethostbyaddr_lru (IP_dst (p));

    if (options.opt_rroute && IP_hl (p) == 15) {
	print_RR (IP_opt (p));
    }

    if (!ICMP_HAS_SEQ (p)) {
	print_icon (ICON_OK);
	goto end_switch;
    }

    if ((ICMP_seq (p) - last_seq) == 0) {
	print_icon (ICON_DUP);
	goto end_switch;
    }

    if (ZOMBIE (p)) {
	print_icon (ICON_ZOMBIE);
	goto end_switch;
    }

    print_icon (ICON_OK);

  end_switch:

    PUTS ("%db from %s", ntohs (IP_len (p)) - (IP_hl (p) << 2), saddr);

    if (mac_inspection)
	PUTS ("[%s]", getmacfromdatalink (p->dl, MAC_SRC));

    if (options.sniff) {
	PUTS (" -> to %s", daddr);

	if (mac_inspection)
	    PUTS ("[%s]", getmacfromdatalink (p->dl, MAC_DST));

    }

    PUTS (": icmp=%d(%s)", ICMP_type (p), icmp_type_str[ICMP_type (p) & 0x3f]);
    if (icmp_code_str[INDEX (ICMP_type (p), ICMP_code (p))] != NULL) {
	PUTS (" code=%d(%s)", ICMP_code (p), icmp_code_str[INDEX (ICMP_type (p), ICMP_code (p))]);
    }
    else {
	if (ICMP_code (p) != 0)
	    PUTS (" code=%d(?)", ICMP_code (p));
    }

    if (ICMP_HAS_SEQ (p)) {
	PUTS (" seq=%d", ICMP_seq (p));
    }

    if (!options.sniff && (ICMP_type (p) == 0 || ICMP_type (p) == 14)) {
	PUTS (" rtt=%ld.%ld ms", rtt.ms_int, rtt.ms_frc);
    }

    if (diff_ipid) {
	PUTS (" ip_id+=%ld\n", diff_id);
    }
    else {
	PUTS (" ip_id=%ld\n", curr_id);
    }

    if (detail < 2)
	return;

    PUTS ("    ttl=%-3d dst=%d(hop)", IP_ttl (p), HOP_DISTANCE (IP_ttl (p)));
    if (ICMP_HAS_ID (p)) {
	PUTS (" icmp_id=%d", ICMP_id (p));
    }

    PUTS (" jitter=%ld ms time_lost=%ld ms\n", jitter, time_lost);
    if (detail < 3)
	return;

    if (icmp_dissect_vector[ICMP_type (p) & 0x3f] != NULL) {
	(*icmp_dissect_vector[ICMP_type (p) & 0x3f]) (p);
    }
    return;

}


void
receiver ()
{
    const u_char *ptr;
    packet *p;
    sigset_t set;
    pcap_t *in_pcap;
    int fval;
    int pcap_fd;

    fval = 0;
    pcap_fd = 0;

    sigemptyset (&set);

    /* masquerading signal */

    sigaddset (&set, SIGTSTP);
    sigaddset (&set, SIGINT);
    sigaddset (&set, SIGQUIT);
    sigaddset (&set, SIGALRM);

    pthread_sigmask (SIG_BLOCK, &set, NULL);

    pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    p = &pkt;

#if defined(__FreeBSD__)
    if ((in_pcap = pcap_open_live (ifname, 1024, options.promisc, 10, bufferr)) == NULL)
#else
    if ((in_pcap = pcap_open_live (ifname, 1024, options.promisc, 0, bufferr)) == NULL)
#endif
	FATAL (bufferr);

    pcap_fd = pcap_fileno (in_pcap);

#ifndef __FreeBSD__
    fcntl (pcap_fd, F_GETFL, fval);
    fval |= O_NONBLOCK;
    fcntl (pcap_fd, F_SETFL, fval);
#endif

    /* set filter */

    if (pcap_compile (in_pcap, &fcode, "icmp", 0, netmask) == -1)
	FATAL ("pcap compile: %s", strerror (errno));

    if (pcap_setfilter (in_pcap, &fcode) == -1)
	FATAL ("pcap_setfilter: %s", strerror (errno));

    /* set offset_dl: datalink header size */

    if (sizeof_datalink (in_pcap) == -1)
	exit (-1);

    if (options.promisc)
	PUTS ("<PROMISC>");

    PUTS ("%s: [%s](%s/%s) with %ld bytes of %s layer.\n", ifname, multi_inet_ntoa ((long) ip_src), multi_inet_ntoa ((long) localnet), multi_inet_ntoa ((long) netmask), offset_dl, linktype[datalink]);

    ENDLESS () {

	DONT_EAT_CPU ();

	ptr = NULL;

	while ((ptr = pcap_next (in_pcap, &pcaphdr)) != (u_char *) NULL) {
	    /* timestamp of current packet */

	    timestamp = (struct timeval *) &(pcaphdr.ts);

	    lineup_layers ((char *) ptr, p);

	    n_recv++;

	    if (GENERIC_FILTER (p) == 0)
		continue;

	    /* the packet fits with filter.. */

	    if (REPLY_FILTER (p))
		n_tome++;

	    switch (ICMP_type (p)) {

	     case ICMP_ECHOREPLY:
		 time_lost = DIFFTIME_int (TVAL_tv (p), last_ack.ts);
		 break;

	     default:
		 time_lost = DIFFTIME_int (last_sent.ts, last_ack.ts);
		 break;

	    }

	    last_ack.ts_sec = timestamp->tv_sec;
	    last_ack.ts_usec = timestamp->tv_usec;

	    if (!options.sniff) {
		agent_timestamp (p);
		agent_dyn_id (p);

		if (agent_ipid (p) != -1 && detail) {
		    process_pack (p);
		}
	    }
	    else if (detail)
		process_pack (p);

	    if (ICMP_HAS_SEQ (p))
		last_seq = ICMP_seq (p);

	    last_id = curr_id;
	    last_rtt.ms_int = rtt.ms_int;

	    last_tstamp = curr_tstamp;

	}
    }
}
