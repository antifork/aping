/*
 * $Id$
 *
 * New aping.
 * 
 * Copyright (c) 2002 Nicola Bonelli <bonelli@antifork.org>
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

#ifndef PROTOTYPE_H
#define PROTOTYPE_H

#include <pcap.h>

char *  multi_inet_ntoa   	(long);
void    usage 	        	(char *, int);
int 	get_first_hop     	(long, long *, char *);
long    gethostbyif		(char *);


long    gethostbyname_lru 	(const char *);
char *  gethostbyaddr_lru 	(unsigned long );
void	receiver		(void);
void    sender            	(char **);
int     sizeof_datalink   	(pcap_t *);
int 	sizeof_icmp		(int);
void    print_icon              (int);       

u_short	chksum                  (u_short * addr, int len);

long 	LOW_PASS_FIR		(long , long );
unsigned char TTL_PREDICTOR	(unsigned char);

void    bandwidth_predictor     (packet *);
long    E                       (long *, long, long);

/* dissect-load */

void   load_echo_reply			(packet *,char **);
void   load_destination_unreachable	(packet *,char **);
void   load_source_quench		(packet *,char **);
void   load_redirect			(packet *,char **);
void   load_echo			(packet *,char **);
void   load_router_advertisement	(packet *,char **);
void   load_router_solicitation		(packet *,char **);
void   load_time_exceeded		(packet *,char **);
void   load_parameter_problem		(packet *,char **);
void   load_timestamp			(packet *,char **);
void   load_timestamp_reply		(packet *,char **);
void   load_information_request		(packet *,char **);
void   load_information_reply		(packet *,char **);
void   load_address_mask_request	(packet *,char **);
void   load_address_mask_reply		(packet *,char **);

void   dissect_echo_reply                  (packet *);
void   dissect_destination_unreachable     (packet *);
void   dissect_source_quench               (packet *);
void   dissect_redirect                    (packet *);
void   dissect_echo                        (packet *);
void   dissect_router_advertisement        (packet *);
void   dissect_router_solicitation         (packet *);
void   dissect_time_exceeded               (packet *);
void   dissect_parameter_problem           (packet *);
void   dissect_timestamp                   (packet *);
void   dissect_timestamp_reply             (packet *);
void   dissect_information_request         (packet *);
void   dissect_information_reply           (packet *);
void   dissect_address_mask_request        (packet *);
void   dissect_address_mask_reply          (packet *);

#endif 
