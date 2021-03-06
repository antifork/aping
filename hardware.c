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
#include "prototype.h"
#include "global.h"
#include "hardware.h"

static
const char cvsid[] = "$Id$";

static char hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

#define nib_h(x)	( (x>>4) & 0x0f )
#define nib_l(x)  	( x & 0x0f )

char *
getmacfrom6b(char *hmac, char *mac)
{
	char *_mac = mac;
	int i = 0;

	while (i < 6) {
		*_mac++ = hex[nib_h(hmac[i])];
		*_mac++ = hex[nib_l(hmac[i++])];
		*_mac++ = ':';
	}
	*--_mac = '\0';

	return mac;
}


char *
getmacfromdatalink(char *dl, int type)
{
	static char mac[18];
	int offset;
	offset = -1;

	switch (datalink) {
	case AP_DLT_EN10MB:
	case AP_DLT_EN3MB:
		if (type == MAC_DST)
			offset = 0;
		else
			offset = 6;
		break;
	case AP_DLT_IEEE802:
	case AP_DLT_FDDI:
		if (type == MAC_DST)
			offset = 2;
		else
			offset = 8;
		break;
//FIXME:	add other interface type
			break;

	}

	if (offset == -1) {
		return ("??:??:??:??:??:??");
	}
	dl += offset;

	return (getmacfrom6b(dl, mac));
}
