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

#include "argscheck.h"
#include "header.h"
#include "typedef.h"
#include "prototype.h"
#include "macro.h"

static
const char cvsid[] = "$Id$";

#ifdef __GNUC__
__inline
#endif
int 
checknum(char *smt)
{
	for (; *smt; smt++)
		if (!isdigit(*smt))
			return 0;
	return 1;
}


#define TEST_OCTET(pivot,smt) \
	if(*smt=='.')smt++; \
	for(pivot=0;pivot!=4;pivot++)octet[pivot]=0; \
        for(pivot=0;isdigit(*smt);pivot++,smt++) \
        { \
		if (pivot >3) return 0; \
        	octet[pivot]=*smt;	 \
        } \
	pivot=strtol(octet, (char **)NULL, 10); \
	if (pivot>255 || pivot < 0 ) return 0; \


#ifdef __GNUC__
__inline
#endif
int 
checkip(char *smt)
{
	char *str, octet[4];
	int pivot, stl, dots = 0;
	for (str = smt, stl = 0; *str; stl++, str++);
	if (isalpha(*smt))
		return 1;
	if (stl > 15)
		return 0;
	if (stl < 7)
		return 0;
	str = smt;
	while (*str) {
		if (*str == '.')
			dots++;
		str++;
	}
	if (dots != 3)
		return 0;

	TEST_OCTET(pivot, smt);
	TEST_OCTET(pivot, smt);
	TEST_OCTET(pivot, smt);
	TEST_OCTET(pivot, smt);
	return 1;
}

#ifdef __GNUC__
__inline
#endif
int
checkport(char *smt)
{
	int a = (int) strtol(smt, (char **) NULL, 10);
	if (a < 1 || a > 65535)
		return 0;
	return 1;
}

#ifdef __GNUC__
__inline
#endif
int
checkportrange(char *smt)
{
	char *pvt;
	char pivot;
	int port;
	int port2;
	for (pvt = smt; *pvt && *pvt != '-'; pvt++);
	if (!*pvt)
		return 0;
	pivot = *pvt;
	*pvt = 0;
	port = strtol(smt, (char **) NULL, 10);
	port2 = strtol(++pvt, (char **) NULL, 10);
	*(--pvt) = pivot;
	if (port > 65535 || port < 1 || port2 > 65535 || port2 < 1)
		return 0;
	return 1;
}

#ifdef __GNUC__
__inline
#endif
int
checkiprange(char *smt)
{
	char *pvt;
	char pivot;
	for (pvt = smt; *pvt && *pvt != '-'; pvt++);
	if (!*pvt)
		return 0;
	pivot = *pvt;
	*pvt = 0;
	checkip(smt);
	checkip(++pvt);
	*(--pvt) = pivot;
	return 1;
}

int
checkargs(char **argv, char minargsnum,...)
{
	va_list ap;
	int paramnum = 0;
	int format;
	char **args;
	args = argv;
	while (*args) {
		paramnum++;
		args++;
	}
	if (paramnum < minargsnum)
		fatal("%d parameters requested", minargsnum);
	va_start(ap, minargsnum);

	for (paramnum = 0; paramnum != minargsnum; paramnum++) {
		format = va_arg(ap, int);
		switch (format) {
		case ARG_NUM:
			if (!checknum(argv[paramnum])) {
				fatal("argument %s is not a valid number", argv[paramnum]);
			}
			break;

		case ARG_IP:
			if (!checkip(argv[paramnum])) {
				fatal("argument %s is not a valid ip\n", argv[paramnum]);
			}
			break;

		case ARG_IP_RANGE:
			if (!checkiprange(argv[paramnum])) {
				fatal("argument %s is not a valid ip or ip_range\n", argv[paramnum]);
			}
			break;

		case ARG_PORT:
			if (!checkport(argv[paramnum])) {
				fatal("argument %s is not a valid port number\n", argv[paramnum]);
			}
			break;

		case ARG_PORT_RANGE:
			if (!checkportrange(argv[paramnum])) {
				fatal("argument %s is not a valid port range\n", argv[paramnum]);
			}
			break;

		case ARG_ANY:
		default:
			break;

		}
	}
	va_end(ap);
	return 1;
}
