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

#include "aping.h"
#include "header.h"
#include "typedef.h"

#include "global.h"
#include "hardware.h"

static char lu_hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

char *
getmacfromdatalink (char *dl, int type)
{
    register int i;
    register char *mac;
    static char *ret;
    int offset;

    offset = -1;
    i = 0;

    mac = ret = (char *) realloc (ret, 18);

    switch (datalink) {

     case AP_IF_ETHER:
	 if (type == MAC_DST)
	     offset = 0;
	 else
	     offset = 6;
	 break;

     case AP_IF_TOKEN:
     case AP_IF_FDDI:
	 if (type == MAC_DST)
	     offset = 2;
	 else
	     offset = 8;

	 break;
     case AP_IF_80211:
     case AP_IF_PPP:
	 // FIXME
	 break;

    }

    if (offset == -1) {
	return ("??:??:??:??:??:??");
    }

    dl += offset;

    while (i < 6) {

	*mac++ = lu_hex[(dl[i] >> 4) & 0xf];
	*mac++ = lu_hex[(dl[i++]) & 0xf];
	*mac++ = ':';

    }

    *--mac = '\0';

    return ret;

}
