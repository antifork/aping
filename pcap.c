/*
 * Copyright (c) 1993, 1994, 1995, 1996, 1997, 1998
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the Computer Systems
 *      Engineering Group at Lawrence Berkeley Laboratory.
 * 4. Neither the name of the University nor of the Laboratory may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "config.h"

#ifdef   HAVE_PCAP_INT_H 
#include <pcap-int.h>
#else
#include "pcap-int.h"
#endif

#ifndef HAVE_PCAP_SETNONBLOCK

int
pcap_setnonblock(pcap_t *p, int nonblock, char *errbuf)
{
        int fdflags;

        if (p->sf.rfile != NULL) {
                /*
                 * This is a savefile, not a live capture file, so
                 * ignore requests to put it in non-blocking mode.
                 */
                return (0);
        }
        fdflags = fcntl(p->fd, F_GETFL, 0);
        if (fdflags == -1) {
                snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "F_GETFL: %s",
                    pcap_strerror(errno));
                return (-1);
        }
        if (nonblock)
                fdflags |= O_NONBLOCK;
        else
                fdflags &= ~O_NONBLOCK;
        if (fcntl(p->fd, F_SETFL, fdflags) == -1) {
                snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "F_SETFL: %s",
                    pcap_strerror(errno));
                return (-1);
        }
        return (0);
}

#endif

#ifndef HAVE_PCAP_GETNONBLOCK

int
pcap_getnonblock(pcap_t *p, char *errbuf)
{
        int fdflags;

        if (p->sf.rfile != NULL) {
                /*
                 * This is a savefile, not a live capture file, so
                 * never say it's in non-blocking mode.
                 */
                return (0);
        }
        fdflags = fcntl(p->fd, F_GETFL, 0);
        if (fdflags == -1) {
                snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "F_GETFL: %s",
                    pcap_strerror(errno));
                return (-1);
        }
        if (fdflags & O_NONBLOCK)
                return (1);
        else
                return (0);
}

#endif
