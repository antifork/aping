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


/***

 aping filtering rules 
 ---------------

 SNIFFER()      : test whether aping runs as sniffer. 
 PINGER()	: test whether aping runs as pinger (!sniffer).
 PROMISC()      : test whether aping set promisc interface.
 ORPHAN()       : test if current icmp is orphan, no request, no reply. 
 PARENT()       : test if current icmp is a reply to our request. 
 TOME()         : test whether dst_ip matches with us.  
 FRME()         : test whether src_ip matches with us.
 MYID()         : test if current icmp_id match with us. 
 ISECHO()       : test if current icmp is an echo.

***/

/* filter rules */
                 
#define rule_0(x)        SNIFFER(x) &&  PROMISC(x)
#define rule_1(x)        SNIFFER(x) && !PROMISC(x) && !FRME  (x)
#define rule_2(x)        PINGER (x) &&  TOME   (x) &&  ORPHAN(x) && RELATED(x)
#define rule_3(x)        PINGER (x) &&  TOME   (x) &&  PARENT(x) && !( ISECHO(x) && !MYID(x) )
                                                                       
/* done */
         
#define GENERIC_FILTER(x)  ((rule_0(x))||(rule_1(x))||(rule_2(x))||(rule_3(x)))
#define REPLY_FILTER(x)    ( rule_3(x) )
