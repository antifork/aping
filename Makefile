#
#  $Id$
#
#  Copyright (c) 2002 Nicola Bonelli <bonelli@antifork.org>
#                     Roberto Ferranti <sbirish@sbirish.net>
#
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are
#  met: 1. Redistributions of source code must retain the above copyright
#  notice, this list of conditions and the following disclaimer. 2.
#  Redistributions in binary form must reproduce the above copyright notice,
#  this list of conditions and the following disclaimer in the documentation
#  and/or other materials provided with the distribution.
#
#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
#  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#  DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
#  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
#  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
#  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
#  SUCH DAMAGE.
#
#

CC      = gcc
DEBUG   = # -DEADBUG 
CFLAGS  = -O3 -funroll-loops -fomit-frame-pointer -pipe -Wall -I.  

LINUX   = -lpcap -lpthread 
OPENBSD = -lpcap  -pthread
FREEBSD = -lpcap  -pthread

INSTALL = /usr/bin/ginstall -c 

OBJS    = aping.o sender.o receiver.o statistic.o fnv.o socket-lru.o iface.o datalink.o \
	argscheck.o \
	resolver.o \
	dissect-destination.o \
	dissect-echo-reply.o \
	dissect-echo.o \
	dissect-information-reply.o \
	dissect-information.o \
	dissect-mask-reply.o \
	dissect-mask.o \
	dissect-parameter-problem.o \
	dissect-redirect.o \
	dissect-router-advertisement.o \
	dissect-router-solicitation.o \
	dissect-source-quench.o \
	dissect-time-exceeded.o \
	dissect-timestamp-reply.o \
	dissect-timestamp.o \
	chksum.o usage.o 

.c.o:
	$(CC) $(CFLAGS) $(DEBUG) -c $< -o $@ 
all :
	@echo "Usage: the following flavors are available:"
	@echo "       make linux"
	@echo "       make openbsd"
	@echo "       make freebsd"


linux:  global 
	$(CC) -o aping $(OBJS) $(LINUX) 	

openbsd:  global
	$(CC) -o aping $(OBJS) $(OPENBSD)

freebsd:  global
	$(CC) -o aping $(OBJS) $(FREEBSD)

global: version.h $(OBJS)

version.o: version.h

version.h: VERSION
	@rm -f $@
	@sed -e 's/.*/#define VERSION "&"/' VERSION > $@
	@echo -n "% program: aping " ; cat VERSION ; 
install:
	$(INSTALL) -c -m 0755 -g bin aping /bin
clean:
	rm -Rf *~ aping *.o version.h
