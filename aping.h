/*
 * $Id$
 *
 * New aping.
 * 
 * Copyright (c) 2002 Nicola Bonelli <bonelli@antifork.org>
 * 
 * All rights reserved.
 * 
 * Linux           : gcc -lpthread -o proxed proxed.c
 * OpenBSD 2.9|3.0 : gcc -pthread -o proxed proxed.c
 * Freebsd 4.4     : gcc -pthread -o proxed proxed.c
 *
 * 
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

#ifndef APING_H
#define APING_H

#ifndef __USE_BSD
#define __USE_BSD
#endif

#define MAX_IPHDR	sizeof(struct ip)+40
#define MAX_ICMPHDR	16 	
#define	MAX_PATTERN	1024
#define MAX_PACSIZE     MAX_IPHDR + MAX_ICMPHDR + sizeof(struct timeval) + MAX_PATTERN	

#define NROUTES         9 /* number of record route slots */

#define ICON_SEND	0
#define ICON_DUP	2
#define ICON_ZOMBIE	3
#define ICON_LOST	4
#define ICON_OK		5

#define USAGE_CLASSIC   0x00
#define USAGE_TOS       0x01
#define USAGE_ICMP_TYPE 0x02
#define USAGE_ICMP_CODE 0x04
#define USAGE_BANDWIDTH 0x08

#endif /* APING_H */
