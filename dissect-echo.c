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
#include "filter.h"

void
load_echo (packet *p, char **argv)
{

  struct timeval now;

  ICMP_type(p)= ICMP_ECHO;
  ICMP_code(p)= 0;
  ICMP_id(p)  = myid;
  ICMP_seq(p) = n_sent;

  memcpy(p->icmp_tstamp_data, pattern, MIN(strlen(pattern),MAX_PATTERN)); 

  gettimeofday(&now,NULL);

  memcpy(p->icmp_tstamp_tval, &now, sizeof(struct timeval));

  ICMP_sum(p)= 0;
  ICMP_sum(p)= chksum((u_short *)p->icmp, sizeof_icmp(ICMP_ECHO));

}

#include "fingerprint.h"

static
char * get_fprint(unsigned long h)
{
   register int i;

   i = 0; 
 
   while ( fingerprint_vector[i].hash )
	{
	   if ( fingerprint_vector[i].hash == h ) 
	         return  fingerprint_vector[i].dscr;
	   i++;
	}

   return "unknown";

}

 
static char fp_buffer[1024];

static void
finger_print(packet *p)
{
  long icmp_len;
  long h; 

  icmp_len    = ntohs (IP_len (p)) - (IP_hl (p) << 2);

  fp_buffer[0]= (char) IP_tos(p);

  fp_buffer[1]= (char)( (icmp_len   ) & 0xff );
  fp_buffer[2]= (char)( (icmp_len>>1) & 0xff );

  fp_buffer[3]= (char) IP_off(p);
  fp_buffer[4]= (char) TTL_PREDICTOR(IP_ttl(p));
  fp_buffer[5]= (char) ICMP_code(p);

  if ( icmp_len > 16 )
	{
	memcpy(fp_buffer+6, TS_DATA(p),icmp_len-16);
  	h = hash (fp_buffer, icmp_len-10); 
	}
  else 
	h = hash (fp_buffer, 8 );

  PUTS("   oTTL=%u", TTL_PREDICTOR(IP_ttl(p)));

  PUTS(" fprint=%lu [%s]\n", h , get_fprint(h));
  
}

void
dissect_echo (packet *p)
{

  finger_print(p);

}
