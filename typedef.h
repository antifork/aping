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

#ifndef TYPEDEF_H
#define TYPEDEF_H

typedef struct {

/* ip */
   int  ip_src:1;
   int  ip_dst:1;

   int  ip_tos:1;
   int  ip_ttl:1;
   int  ip_ramp:1;
   int  ip_id:1;
   int  ip_id_incr:1;
   int  ip_id_rand:1;
   int  opt_rroute:1;

/* icmp */
   int  icmp_type:1;
   int  icmp_code:1;

/* host */
   int  ifname:1;
   int  wait:1;
   int  count:1;
   int  pattern:1;
   int  numeric:1;
   int  droute:1;
   int  so_debug:1;
   int  detail:1;
   int  dfrag:1;
   int  flood:1;
   int  sniff:1;
   int  size:1;
   int  promisc:1;

} OPT;

typedef union {

  struct {		
	long sec;
	long usec;
  } ts;

#define ts_sec  ts.sec
#define ts_usec ts.usec

  struct {
	long i;
	long f;

  } ms;		

#define ms_int  ms.i
#define ms_frc  ms.f

} TIME;


typedef struct {
        u_char	       *dl; 
     	struct ip      *ip;
        struct icmp    *icmp;

        u_char 	       *data;
   	struct { 
        	struct timeval *tval;
        	char           *data;
		} echo; 

#define icmp_tstamp_data echo.data 
#define icmp_tstamp_tval echo.tval 

} packet;


typedef struct {
	long kbit;
	char *type;
} flink;


typedef struct {
	long lenght;
   	char *type;
} ipl_stat;


#endif /* typedef */

