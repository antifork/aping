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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS …AS IS'' AND ANY
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

/* internals */

#define IP_ver(p)	((p)->ip->ip_v)
#define IP_hl(p)        ((p)->ip->ip_hl)
#define IP_tos(p)	((p)->ip->ip_tos)
#define IP_len(p)       ((p)->ip->ip_len)
#define IP_id(p)        ((p)->ip->ip_id)
#define IP_off(p)	((p)->ip->ip_off)
#define IP_ttl(p)       ((p)->ip->ip_ttl)
#define IP_p(p)		((p)->ip->ip_p)
#define IP_sum(p)	((p)->ip->ip_sum)
#define IP_src(p)	((p)->ip->ip_src.s_addr)
#define IP_dst(p)	((p)->ip->ip_dst.s_addr)

#define IP_opt(p)	((char *)((p)->ip)+20)

#define ICMP_type(p)    ((p)->icmp->icmp_type)
#define ICMP_code(p)	((p)->icmp->icmp_code)
#define ICMP_sum(p)     ((p)->icmp->icmp_cksum)
#define ICMP_gwaddr(p)  ((p)->icmp->icmp_gwaddr.s_addr)
#define ICMP_id(p)	((p)->icmp->icmp_id) 
#define ICMP_seq(p)	((p)->icmp->icmp_seq)

#define ICMP_IP_ver(p)  ((p)->icmp->icmp_ip.ip_v)
#define ICMP_IP_hl(p)   ((p)->icmp->icmp_ip.ip_hl)
#define ICMP_IP_tos(p)  ((p)->icmp->icmp_ip.ip_tos)
#define ICMP_IP_len(p)  ((p)->icmp->icmp_ip.ip_len)
#define ICMP_IP_id(p)   ((p)->icmp->icmp_ip.ip_id)
#define ICMP_IP_off(p)  ((p)->icmp->icmp_ip.ip_off)
#define ICMP_IP_ttl(p)  ((p)->icmp->icmp_ip.ip_ttl)
#define ICMP_IP_p(p)    ((p)->icmp->icmp_ip.ip_p)
#define ICMP_IP_sum(p)  ((p)->icmp->icmp_ip.ip_sum)
#define ICMP_IP_src(p)  ((p)->icmp->icmp_ip.ip_src.s_addr)
#define ICMP_IP_dst(p)  ((p)->icmp->icmp_ip.ip_dst.s_addr)

#define ICMP_TCP_sport(p) (*(short*)((p)->data+0+(p)->icmp->icmp_ip.ip_hl*4)) 
#define ICMP_TCP_dport(p) (*(short*)((p)->data+2+(p)->icmp->icmp_ip.ip_hl*4)) 
#define ICMP_TPC_ns(p)	  (*(u_int32_t*)((p)->data+4+(p)->icmp->icmp_ip.ip_hl*4))

#define ICMP_UDP_sport(p)  (*(short*)((p)->data+0+(p)->icmp->icmp_ip.ip_hl*4))
#define ICMP_UDP_dport(p)  (*(short*)((p)->data+2+(p)->icmp->icmp_ip.ip_hl*4))
#define ICMP_UDP_len(p)    (*(short*)((p)->data+4+(p)->icmp->icmp_ip.ip_hl*4))
#define ICMP_UDP_chksum(p) (*(short*)((p)->data+6+(p)->icmp->icmp_ip.ip_hl*4))

#define ICMP_ICMP_type(p) ((p)->data[0+ ((p)->icmp->icmp_ip.ip_hl*4)])
#define ICMP_ICMP_code(p) ((p)->data[1+ ((p)->icmp->icmp_ip.ip_hl*4)])

#define ICMP_ICMP_sum(p)  (*(short*)((p)->data+2+(p)->icmp->icmp_ip.ip_hl*4)) 
#define ICMP_ICMP_id(p)   (*(short*)((p)->data+4+(p)->icmp->icmp_ip.ip_hl*4)) 
#define ICMP_ICMP_seq(p)  (*(short*)((p)->data+6+(p)->icmp->icmp_ip.ip_hl*4)) 

#define TVAL_sec(p)	((p)->icmp_tstamp_tval->tv_sec)
#define TVAL_usec(p)	((p)->icmp_tstamp_tval->tv_usec)
#define TVAL_tv(p)	(p)->icmp_tstamp_tval->tv

#define RAW_DATA(p)	((p)->data)
#define TS_DATA(p)	((p)->icmp_tstamp_data)
 

/* filters macro */

#define SNIFFER(p)       ( options.sniff )
#define PINGER(p)	 (!options.sniff )
#define PROMISC(p)	 ( options.promisc )
#define ORPHAN(p)        ( icmp_parent[ICMP_type(p) & 0x1f] == -1 )

#define RELATED(p)       ( ICMP_IP_src(p)    == ip_src && \
			   ICMP_IP_dst(p)    == ip_dst && \
			   ICMP_IP_p(p)      == IPPROTO_ICMP && \
			   ICMP_ICMP_type(p) == icmp_type && \
			   ICMP_ICMP_code(p) == icmp_code && \
		        !( ICMP_ICMP_type(p) == 0 && !(ICMP_ICMP_id(p)== myid) ) )

#define PARENT(p)        ( icmp_parent[ICMP_type(p) & 0x1f] == icmp_type )
#define TOME(p)	 	 ( IP_dst(p) == ip_src )
#define FRME(p)		 ( IP_src(p) == ip_src )
#define MYID(p)          ( ICMP_id(p) == myid )
#define ISECHO(p)        ( ICMP_type(p) == 0  )

#define ICMP_HAS_SEQ(p)	 ( ICMP_type (p) == 0  || ICMP_type (p) == 8  ||  \
                           ICMP_type (p) == 13 || ICMP_type (p) == 14 ||  \
                           ICMP_type (p) == 15 || ICMP_type (p) == 16     )

#define ICMP_HAS_TSTAMP(p) ( ICMP_type(p) == 0 || ICMP_type (p) == 14 )

#define ICMP_HAS_ID(p)	ICMP_HAS_SEQ(p)

#define ZOMBIE(p)	((ICMP_seq(p)-n_sent)< -1)

#include "user-filter.h"
 
