#
#  $Id$
#  <A PING. The raw-ping>
#
#  Copyright (c) 1999 Bonelli Nicola <awgn@antifork.org>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#

CC      = gcc
DEBUG   = # -DEADBUG 
CFLAGS  = -O3 -funroll-loops -fomit-frame-pointer -pipe -Wall -I.  

LINUX   = -lpcap -lpthread 
OPENBSD = -lpcap  -pthread
FREEBSD = -lpcap  -pthread

INSTALL = /usr/bin/ginstall -c 

OBJS    = aping.o sender.o receiver.o statistic.o socket-lru.o iface.o datalink.o \
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
