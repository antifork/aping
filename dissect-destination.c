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

/* ICMP_DEST_UNREACH 
     
   Destination Unreachable Message

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Type      |     Code      |          Checksum             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                             unused                            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |      Internet Header + 64 bits of Original Data Datagram      |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


*/
 
#include "dissect.h" 
#include "aping.h"

#include "typedef.h"
#include "prototype.h"
#include "global.h"
#include "argscheck.h"


/*

Destination Unreachable Message

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Type      |     Code      |          Checksum             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                             unused                            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |      Internet Header + 64 bits of Original Data Datagram      |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

*/

static int _dissect_type = ICMP_UNREACH;
#include "maturity.h"

void
load_destination_unreachable(packet *p, char **argv)
{

  	/* maturity level */
        SET_LOADER_LEVEL('b');

	/* destination, redirect_type , tos,icmp_src_ip, icmp_dest_ip src_port, dest_port*/
	checkargs(argv,5,ARG_IP,ARG_NUM,ARG_NUM,ARG_IP,ARG_IP,ARG_PORT,ARG_PORT);

	ICMP_type(p)= ICMP_UNREACH;
	ICMP_code(p)= ((int)strtol(argv[1], (char **)NULL, 10))%4;

	ICMP_IP_ver(p)  = 4; 
	ICMP_IP_hl(p)   = 5; /*no options*/
	ICMP_IP_tos(p)  = ((int)strtol(argv[2], (char **)NULL, 10));
	ICMP_IP_len(p)  = htons(sizeof(struct ip)+64+20);
	ICMP_IP_id(p)   = htons(1+(int) (65535.0*rand()/(RAND_MAX+1.0))); 
	ICMP_IP_off(p)  = 0;
	ICMP_IP_ttl(p)  = 255;
	ICMP_IP_p(p)    = IPPROTO_TCP;
	ICMP_IP_src(p)  = gethostbyname_lru(argv[3]);
	ICMP_IP_dst(p)  = gethostbyname_lru(argv[4]);

        ICMP_IP_sum(p)  = 0;
        ICMP_IP_sum(p)  = chksum ((u_short *)ICMP_IP(p),ICMP_IP_hl(p) << 2);

	ICMP_TCP_sport(p) = htons((short)strtol(argv[5], (char **)NULL, 10)); 
	ICMP_TCP_dport(p) = htons((short)strtol(argv[6], (char **)NULL, 10));

	ICMP_sum(p)= 0;
	ICMP_sum(p)= chksum((u_short *)p->icmp, sizeof_icmp(ICMP_UNREACH));

}

void
dissect_destination_unreachable(packet *p)
{

	/* maturity level */
  	SET_DISSECT_LEVEL('*');

	PRINT_IPH_64bit(p);
	PUTS("\n");

	if ( detail < 3 ) 
		return;

	bandwidth_predictor(p);

}
