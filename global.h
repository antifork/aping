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

#include <termios.h>
#include <unistd.h>
#include <pcap.h>
#include <pthread.h>

#include "aping.h"
#include "hardware.h"

#ifdef  GLB_OWNER
#define EXTERN
#define INIT(x...) x
#else
#define EXTERN extern
#define INIT(x...)
#endif

EXTERN OPT  options;

EXTERN long ip_id;
EXTERN long ip_src;
EXTERN long ip_dst;
EXTERN long ip_tos;
EXTERN long ip_ttl INIT(=255);		/* default ttl */
EXTERN long icmp_type INIT(=8);		/* default echo request */
EXTERN long icmp_code;

EXTERN long datalink;			/* datalink type */
EXTERN long mac_inspection;

EXTERN long n_sent;
EXTERN long n_recv;
EXTERN long n_tome;
EXTERN long n_pause;

EXTERN long myid;			/* id used to mark icmp */

EXTERN long tau INIT(= 1000);        	/* msec between each packet */

EXTERN long mysize;
EXTERN long detail INIT (=1);		/* level */
EXTERN long sniff;      		/* sniff option */
EXTERN long count;      		/* max number of packets */
EXTERN long offset_dl;

EXTERN long last_route; 		/* last rroute signature */

EXTERN TIME rtt;        
EXTERN TIME last_rtt;
EXTERN TIME last_ack;
EXTERN TIME last_sent;

EXTERN long last_tstamp;
EXTERN long curr_tstamp; 

EXTERN long rtt_min INIT(= 99999999);
EXTERN long rtt_max;
EXTERN long rtt_mean;   		/* E{rtt} */
EXTERN long rtt_sqre;   		/* E{rtt^2} */ 


EXTERN long jitter;
EXTERN long time_lost;  		/* due to packets lost */

EXTERN long traffic_tos;

EXTERN long endian_bug;        		/* ip_id endianess bug */
EXTERN long diff_ipid;			/* ip_id differential */

EXTERN long last_seq INIT(= -1);      	/* last icmp_seq number received */
EXTERN long last_id  INIT(= -1);    

EXTERN long curr_id;
EXTERN long diff_id;

EXTERN long out_burst;  		/* the current remote outgoing burst in kbit */
EXTERN long max_burst;  		/* the max recorded burst ( used to predict the bandwidth) */
EXTERN long mean_burst;

EXTERN long delta;
EXTERN long pending_time;
EXTERN long local_tau;

EXTERN long slow_start INIT(= 1); 	/* to calc the correct value of ip_id */ 

EXTERN long rand_ip_id; 
EXTERN long ipid_failure;		/* used by agent endian bug */

EXTERN bpf_u_int32 localnet;
EXTERN bpf_u_int32 netmask;

EXTERN char *host_src;
EXTERN char *host_dst;
EXTERN char *hw_src;
EXTERN char *hw_dst;
EXTERN char *pattern;

EXTERN char  maturity_level[64][2];
EXTERN char  maturity_void [0];

EXTERN char buffer [MAX_PACSIZE];
EXTERN char bufferr[PCAP_ERRBUF_SIZE];

EXTERN char ifname [16];

EXTERN pthread_t       pd_rcv;
EXTERN pthread_t       pd_snd;
EXTERN pthread_t       pd_key;

EXTERN long            pd_plugin[MAX_CHILD];
EXTERN long	       pd_pindex;

EXTERN struct termios  termios_p;

EXTERN void  (*icmp_loader_vector[64]) (packet *, char **) INIT( =
{
        [0] =load_echo_reply,
        [3] =load_destination_unreachable,
        [4] =load_source_quench,
        [5] =load_redirect,
        [8] =load_echo,
        [9] =load_router_advertisement,
        [10]=load_router_solicitation,
        [11]=load_time_exceeded,
        [12]=load_parameter_problem,
        [13]=load_timestamp,
        [14]=load_timestamp_reply,
        [15]=load_information_request,
        [16]=load_information_reply,
        [17]=load_address_mask_request,
        [18]=load_address_mask_reply,
	[40]=load_security
});

EXTERN void  (*icmp_dissect_vector[64]) (packet *) INIT (=
{
        [0] =dissect_echo_reply,
        [3] =dissect_destination_unreachable,
        [4] =dissect_source_quench,
        [5] =dissect_redirect,
        [8] =dissect_echo,
        [9] =dissect_router_advertisement,
        [10]=dissect_router_solicitation,
        [11]=dissect_time_exceeded,
        [12]=dissect_parameter_problem,
        [13]=dissect_timestamp,
        [14]=dissect_timestamp_reply,
        [15]=dissect_information_request,
        [16]=dissect_information_reply,
        [17]=dissect_address_mask_request,
        [18]=dissect_address_mask_reply,
	[40]=dissect_security
});

EXTERN char *linktype[16]   INIT( = {
	[AP_IF_UNKNOWN]= "unknown",
	[AP_IF_ETHER  ]= "ethernet",
	[AP_IF_TOKEN  ]= "tokenring",
	[AP_IF_FDDI   ]= "fddi",
	[AP_IF_80211  ]= "802.11 wireless",
	[AP_IF_PPP    ]= "ppp" } );

EXTERN char *protocols[256] INIT( = {
[0]="ip", [1]="icmp", [2]="igmp", [3]="ggp", [4]="ipencap",
[5]="st", [6]="tcp", [7]="ucl", [8]="egp", [9]="igp",
[10]="bbn-rcc-mon", [11]="nvp-ii", [12]="pup", [13]="argus", [14]="emcon",
[15]="xnet", [16]="chaos", [17]="udp", [18]="mux", [19]="dcn-meas", [20]="hmp",
[21]="prm", [22]="xns-idp", [23]="trunk-1", [24]="trunk-2", [25]="leaf-1", [26]="leaf-2",
[27]="rdp", [28]="irtp", [29]="iso-tp4", [30]="netblt", [31]="mfe-nsp", [32]="merit-inp",
[33]="sep", [34]="3pc", [35]="idpr", [36]="xtp", [37]="ddp", [38]="idpr-cmtp", [39]="idpr-cmtp",
[40]="il", [41]="ipv6", [42]="sdrp", [43]="sip-sr", [44]="sip-frag", [45]="idrp", [46]="rsvp",
[47]="gre", [48]="mhrp", [49]="bna", [50]="esp", [51]="ah", [52]="i-nlsp", [53]="swipe", [54]="nhrp",
[55]="mobileip", [57]="skip", [58]="ipv6-icmp", [59]="ipv6-nonxt", [60]="ipv6-opts", [61]="any", [62]="cftp",
[63]="any", [64]="sat-expak", [65]="kryptolan", [66]="rvd", [67]="ippc", [68]="any", [69]="sat-mon",
[70]="visa", [71]="ipcv", [72]="cpnx", [73]="cphb", [74]="wsn", [75]="pvp", [76]="br-sat-mon", [77]="sun-nd",
[78]="wb-mon", [79]="wb-expak", [80]="iso-ip", [81]="vmtp", [82]="secure-vmtp", [83]="vines", [84]="ttp",
[85]="nsfnet-igp", [86]="dgp", [87]="tcf", [88]="igrp", [89]="ospf", [90]="sprite-rpc", [91]="larp",
[92]="mtp", [93]="ax.25", [94]="ipip", [95]="micp", [96]="scc-sp", [97]="etherip", [98]="encap",
[99]="any", [100]="gmtp", [103]="pim", [108]="ipcomp", [112]="vrrp", [255]="reserved"
}) ;

#undef EXTERN
#undef INIT 
