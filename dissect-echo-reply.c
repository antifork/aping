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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS �AS IS'' AND ANY
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
               
Echo or Echo Reply Message
   
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Type      |     Code      |          Checksum             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |           Identifier          |        Sequence Number        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Data ...
   +-+-+-+-+-
   
*/

 
#include "dissect.h" 
#include "aping.h"

#include "typedef.h"
#include "prototype.h"
#include "global.h"

#include "prototype.h"
#include "filter.h"
#include "argscheck.h"

void
load_echo_reply (packet *p, char **argv)
{
	/* id, seq */
	checkargs(argv,2,ARG_NUM,ARG_NUM);

	ICMP_type(p)= 0; /*echo reply */
	ICMP_code(p)= 0;
	ICMP_id(p)  = htons(((int)strtol(argv[0], (char **)NULL, 10)));
	ICMP_seq(p) = htons(((int)strtol(argv[1], (char **)NULL, 10)));

	/*since bsd kernel don't give shit about 64 bits we don't care and fill with shit */
	ICMP_TCP_sport(p) = htons(1+(int) (65535.0*rand()/(RAND_MAX+1.0))); 
	ICMP_TCP_dport(p) = htons((short)(ICMP_TCP_sport(p)+0xfaded));


	ICMP_sum(p)= 0;
	ICMP_sum(p)= chksum((u_short *)p->icmp, sizeof_icmp(ICMP_ECHO));	
}


void
dissect_echo_reply (packet *p)
{

  bandwidth_predictor(p);  

}
