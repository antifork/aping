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

extern int offset_dl;
extern int datalink;

#define CASE(x,size,type) {	\
case (x):			\
offset_dl=(size);		\
datalink =(type);		\
break;}

int
sizeof_datalink (pcap_t * pd)
{
    register int dtl;

    if ((dtl = pcap_datalink (pd)) < 0)
	FATAL ("no datalink info: %s", pcap_geterr (pd));

    switch (dtl) {

	 CASE (DLT_NULL, 4, AP_IF_UNKNOWN);
	 CASE (DLT_EN10MB, 14, AP_IF_ETHER);
#ifdef DLT_EN3MB
	 CASE (DLT_EN3MB, 14, AP_IF_UNKNOWN);
#endif
#ifdef DLT_AX25
	 CASE (DLT_AX25, -1, AP_IF_UNKNOWN);
#endif
#ifdef DLT_PRONET
	 CASE (DLT_PRONET, -1, AP_IF_UNKNOWN);
#endif
#ifdef DLT_CHAOS
	 CASE (DLT_CHAOS, -1, AP_IF_UNKNOWN);
#endif

	 CASE (DLT_IEEE802, 22, AP_IF_TOKEN);
	 CASE (DLT_ARCNET, -1, AP_IF_UNKNOWN);

     case DLT_SLIP:
#ifdef DLT_SLIP_BSDOS
     case DLT_SLIP_BSDOS:
#endif
	 datalink = AP_IF_SLIP;
#if (FREEBSD || OPENBSD || NETBSD || BSDI)
	 offset_dl = 16;
#else
	 offset_dl = 24;	/* Anyone use this??? */
#endif

     case DLT_PPP:
#ifdef DLT_PPP_BSDOS
     case DLT_PPP_BSDOS:
#endif
#if defined ( __FreeBSD__ ) || defined (__OpenBSD__) || defined(__NetBSD__)
	 offset_dl = 4;
	 datalink = AP_IF_PPP;
#else
#if defined (__solaris__)
	 offset_dl = 8;
	 datalink = AP_IF_PPP;
#else
	 offset_dl = 24;
	 datalink = AP_IF_PPP;
#endif
#endif
	 break;

	 CASE (DLT_FDDI, 24, AP_IF_FDDI);

#ifdef DLT_ATM_RFC1483
	 CASE (DLT_ATM_RFC1483, 8, AP_IF_UNKNOWN);
#endif
#ifdef DLT_RAW
	 CASE (DLT_RAW, 0, AP_IF_UNKNOWN);
#endif

#ifdef DLT_IEEE802_11
	 CASE (DLT_IEEE802_11, 30, AP_IF_UNKNOWN);
#endif

#ifdef  DLT_LOOP
	 CASE (DLT_LOOP, 4, AP_IF_UNKNOWN);
#endif

#ifdef DLT_ENC
	 CASE (DLT_ENC, 12, AP_IF_UNKNOWN);
#endif

#ifdef DLT_LINUX_SLL
	 CASE (DLT_LINUX_SLL, 16, AP_IF_PPP);
#endif

#ifdef DLT_PRISM_HEADER
	 CASE (DLT_PRISM_HEADER, 30, AP_IF_UNKNOWN);
	 break;
#endif
#ifdef DLT_AIRONET_HEADER
	 CASE (DLT_AIRONET_HEADER, 30, AP_IF_UNKNOWN);
	 break;
#endif

     default:
	 FATAL ("unknown datalink type DTL_?=%d", dtl);
	 break;
    }

    return 0;
}
