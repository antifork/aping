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

#include <pcap.h>
#include <pthread.h>

#include "aping.h"

#ifdef  GLOBAL_HERE
#define EXTERN	
#else
#define EXTERN extern	
#endif 

EXTERN OPT  options;

EXTERN long ip_id;
EXTERN long ip_src;
EXTERN long ip_dst;
EXTERN long ip_tos;
EXTERN long ip_ttl;
EXTERN long icmp_type;
EXTERN long icmp_code;

EXTERN long n_sent;
EXTERN long n_recv;
EXTERN long n_tome;

EXTERN long myid;	/* id used to mark icmp */

EXTERN long tau;        /* msec between each packet */

EXTERN long mysize;
EXTERN long verbose;    /* level */
EXTERN long sniff;      /* sniff option */
EXTERN long count;      /* max number of packets */
EXTERN long offset_dl;

EXTERN long last_route; /* last rroute signature */

EXTERN TIME rtt;        
EXTERN TIME last_rtt;
EXTERN TIME last_ack;
EXTERN TIME last_sent;

EXTERN long rtt_min;
EXTERN long rtt_max;
EXTERN long rtt_mean;   /* E{rtt} */
EXTERN long rtt_sqre;   /* E{rtt^2} */ 


EXTERN long jitter;
EXTERN long time_lost;  /* due to packets lost */

EXTERN long traffic_tos;

EXTERN long iddeal;        /* ip_id endianess bug */

EXTERN long last_seq;      /* last icmp_seq number received */

EXTERN long last_id;    
EXTERN long curr_id;
EXTERN long diff_id;

EXTERN long out_burst;  /* the current remote outgoing burst in kbit */
EXTERN long max_burst;  /* the max recorded burst ( used to predict the bandwidth) */
EXTERN long mean_burst;

EXTERN long slow_start; /* to avoid big burst during the ip_id mode switching ( cltr+\ )*/ 

EXTERN long rand_ip_id; 
EXTERN long fail_ip_id;

EXTERN bpf_u_int32 localnet;
EXTERN bpf_u_int32 netmask;


EXTERN char *host_src;
EXTERN char *host_dst;
EXTERN char *pattern;

EXTERN char buffer [MAX_PACSIZE];
EXTERN char bufferr[PCAP_ERRBUF_SIZE];

EXTERN char rspace [3 + 4 * NROUTES + 1];
EXTERN char filter [512];
EXTERN char ifname [16];

EXTERN pthread_t       pd_rcv;
EXTERN pthread_t       pd_snd;

EXTERN pthread_mutex_t pd_mutex; 

#undef EXTERN
