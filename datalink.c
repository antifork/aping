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


#include <pcap.h>
#include "macro.h"
#include "hardware.h"

extern int    offset_dl;
extern int    datalink;

int
sizeof_datalink (pcap_t * pd)
{
    register int  dtl;

    if ((dtl = pcap_datalink (pd)) < 0)
	FATAL ("no datalink info: %s", pcap_geterr (pd));

    switch (dtl)
	{
#ifdef DLT_NULL
	 case DLT_NULL:
	     offset_dl = 4;
	     datalink  = AP_IF_UNKNOWN;
	     break;
#endif

	 case DLT_PPP:
#ifdef DLT_PPP_BSDOS
	 case DLT_PPP_BSDOS:
#endif
#if defined ( __FreeBSD__ ) || defined (__OpenBSD__) || defined(__NetBSD__) 
	     offset_dl = 4;
	     datalink  = AP_IF_UNKNOWN;
#else
#if defined (__solaris__)
	     offset_dl = 8;
	     datalink  = AP_IF_UNKNOWN;
#else
	     offset_dl = 24;
	     datalink  = AP_IF_UNKNOWN;
#endif
#endif
	     break;

	 case DLT_SLIP:
#ifdef DLT_SLIP_BSDOS
	 case DLT_SLIP_BSDOS:
#endif
#if defined ( __FreeBSD__ ) || defined (__OpenBSD__) || defined(__NetBSD__)
	     offset_dl = 16;
	     datalink  = AP_IF_UNKNOWN;
#else
	     offset_dl = 24;
	     datalink  = AP_IF_UNKNOWN;
#endif
	     break;

	     /*  /usr/include/net/bpf.h Berkeley 8.[12] ver.  */
#ifdef  DLT_RAW
	 case DLT_RAW:
	     offset_dl = 0;
	     datalink  = AP_IF_UNKNOWN;
	     break;
#endif
#ifdef  DLT_LOOP
	 case DLT_LOOP:
	     offset_dl = 4;
	     datalink  = AP_IF_UNKNOWN;
	     break;
#endif

#ifdef DLT_ENC
	 case DLT_ENC:
	     offset_dl = 12;
	     datalink  = AP_IF_UNKNOWN;
	     break;
#endif

	 case DLT_FDDI:
	     offset_dl = 21;
	     datalink  = AP_IF_FDDI;
	     break;
	 case DLT_EN10MB:
	     offset_dl = 14;
	     datalink  = AP_IF_ETHER;
	     break;
	 case DLT_ATM_RFC1483:
	     offset_dl = 8;
	     datalink  = AP_IF_UNKNOWN; 
	     break;
	 case DLT_IEEE802:
	     /* 802.5 token ring */
	     offset_dl = 22;
	     datalink  = AP_IF_TOKEN;
	     break;

#ifdef DLT_IEEE802_11
	/* wireless */
             offset_dl = 30;
             datalink  = AP_IF_80211;
             break;
#endif
	 default:
	     FATAL ("unknown datalink type (%d)", datalink);
	     break;
	}

    return 0;
}
