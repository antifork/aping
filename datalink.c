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


#include  <pcap.h>
#include "macro.h"
#include "hardware.h"
#include "bpf.h"

extern int offset_dl;
extern int datalink;

#define CASE(x,y) {		\
case (x):			\
offset_dl=(y);			\
datalink =(x);			\
break;}

int
sizeof_datalink (pcap_t * pd)
{
    register int dtl;

    if ((dtl = pcap_datalink (pd)) < 0)
	FATAL ("no datalink info: %s", pcap_geterr (pd));

    switch (dtl) {

	 CASE (AP_DLT_NULL,4);
	 CASE (AP_DLT_EN10MB,14);
	 CASE (AP_DLT_EN3MB,14);
	 CASE (AP_DLT_AX25,-1);
	 CASE (AP_DLT_PRONET,-1);
	 CASE (AP_DLT_CHAOS,-1);
	 CASE (AP_DLT_IEEE802,22);
	 CASE (AP_DLT_ARCNET,-1);
#if defined (__FreeBSD__) || defined (__OpenBSD__) || defined (__NetBSD__) || defined (__BSDI__) 
	 CASE (AP_DLT_SLIP,16);
#else
	 CASE (AP_DLT_SLIP,24);
#endif

#if defined (__FreeBSD__) || defined (__OpenBSD__) || defined (__NetBSD__)
	 CASE (AP_DLT_PPP,4);
#elif defined (__sun)
	 CASE (AP_DLT_PPP,8);
#else
	 CASE (AP_DLT_PPP,24);
#endif
	 CASE (AP_DLT_FDDI,21);
	 CASE (AP_DLT_ATM_RFC1483,8);

#if defined (__OpenBSD__)
	 CASE (AP_DLT_RAW,4);		/* according to tun interface */
#else
	 CASE (AP_DLT_RAW,0);		/* according to nmap 3.00 */
#endif
	 CASE (AP_DLT_SLIP_BSDOS,16);
	 CASE (AP_DLT_PPP_BSDOS,4);
	 CASE (AP_DLT_ATM_CLIP,-1);
#if defined (__FreeBSD__) || defined (__OpenBSD__) || defined (__NetBSD__)
         CASE (AP_DLT_PPP_SERIAL,4);
	 CASE (AP_DLT_PPP_ETHER, 4);
#elif defined (__sun)
         CASE (AP_DLT_PPP_SERIAL,8);
         CASE (AP_DLT_PPP_ETHER, 8);
#else
         CASE (AP_DLT_PPP_SERIAL,24);
         CASE (AP_DLT_PPP_ETHER, 24);
#endif
	 CASE (AP_DLT_C_HDLC,-1);
	 CASE (AP_DLT_IEEE802_11,30);
	 CASE (AP_DLT_LOOP,4);
	 CASE (AP_DLT_LINUX_SLL,16);
	 CASE (AP_DLT_LTALK,-1);
	 CASE (AP_DLT_ECONET,-1);
	 CASE (AP_DLT_IPFILTER,-1);
	 CASE (AP_DLT_PFLOG,-1);
	 CASE (AP_DLT_CISCO_IOS,-1);
	 CASE (AP_DLT_PRISM_HEADER,-1);
	 CASE (AP_DLT_AIRONET_HEADER,-1);

     default:
	 FATAL ("unknown datalink type DTL_?=%d", dtl);
	 break;
    }

    return offset_dl;
}
