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

PLUGINS	= loader dummy

all: banner plugins
banner:
	@echo "=================="
	@echo "  making plugins"
	@echo "=================="

plugins:
	for plug in $(PLUGINS); do \
	cd $$plug && make ; cd .. ; \
	done

clean-local:
	@rm -Rf config.log config.status

clean: clean-local
	for plug in $(PLUGINS); do \
	cd $$plug && make clean; cd .. ; \
	done

distclean: clean-local
	for plug in $(PLUGINS); do \
	cd $$plug && make distclean; cd .. ; \
	done

install:
	for plug in $(PLUGINS); do \
	cd $$plug && make install; cd .. ; \
	done
