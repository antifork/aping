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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS .AS IS'' AND ANY
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

#ifndef HARDWARE_H
#define HARDWARE_H

#define MAC_SRC		0
#define MAC_DST		1

#define AP_IF_UNKNOWN   0
#define AP_IF_ETHER	1
#define AP_IF_TOKEN	2
#define AP_IF_FDDI	3
#define AP_IF_80211	4
#define AP_IF_PPP	5	
#define AP_IF_SLIP	6

#define ETHER_ADDR_LEN	   6
#define ISO88025_ADDR_LEN  6


/* Ethernet physical header */

struct ether_header {
	u_int8_t	ether_dhost[ETHER_ADDR_LEN];
	u_int8_t	ether_shost[ETHER_ADDR_LEN];
	u_int16_t 	ether_type;
};

/* Token Ring physical header */

struct token_header {
        u_int8_t  token_ac;                     	/* access control field */
        u_int8_t  token_fc;                     	/* frame control field */
        u_int8_t  token_dhost[ISO88025_ADDR_LEN];       /* dest. address */
        u_int8_t  token_shost[ISO88025_ADDR_LEN];       /* source address */
} __attribute__((__packed__));   


/* Structure of an 100Mb/s FDDI header.  */

struct  fddi_header {
        u_char  fddi_fc;
        u_char  fddi_dhost[6];
        u_char  fddi_shost[6];
};


#endif /* HARDWARE_H */	
