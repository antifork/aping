/*
 * $Id$
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

/* msg from rcf792/1700/1716/1256 */

#ifndef HAVE_ICMP_DEF
#define HAVE_ICMP_DEF 

char *icmp_type_str[32] =
{ "echo", NULL, NULL, "unreach", "squench", "redirect", NULL, "r_route",
  "echo_RQ", "routeradver", "routersolicit", "t_xceed", "paramprb", "tstamp",
  "tstamp_rp", "info_RQ", "info_rp", "mask_RQ", "mask_rp",
};

#define str_code(a,b,s) [a*32+b] s

char *icmp_code_str[32*32] =
{
str_code(3, 0, "net unreachable"),
str_code(3, 1, "host unreachable"),
str_code(3, 2, "protocol unreachable"),
str_code(3, 3, "port unreachable"),
str_code(3, 4, "fragmentation needed and DF set"),
str_code(3, 5, "source route failed"),
str_code(3, 6, "unknown network"),
str_code(3, 7, "unknown host"),
str_code(3, 8, "host isolated"),
str_code(3, 9, "network prohib (crypto devs)"),
str_code(3,10, "host prohib (ditto)"),
str_code(3,11, "bad tos for net"),
str_code(3,12, "bad tos for host"),
str_code(3,13, "packet filtered"),
str_code(3,14, "precedence violation"),
str_code(3,15, "precedence cuf off"),
str_code(4, 0, "source quench"),
str_code(5, 0, "redirect datagr. for net"),
str_code(5, 1, "redirect datagr. for host"),
str_code(5, 2, "redirect datagr. for tos and net"),
str_code(5, 3, "redirect datagr. for tos and host"),
str_code(9, 0, "router advertisement"),
str_code(11,0, "ttl exceeded in transit"),
str_code(11,1, "fragment reassembly exceeded"),
str_code(12,0, "pointer indicates the error"),
str_code(13,0, "timestamp"),
str_code(14,0, "timestamp reply"),
str_code(15,0, "information request"),
str_code(16,0, "information reply"),
str_code(17,0, "mask request"),
str_code(18,0, "mask reply"),

};


#endif /* HAVE_ICMP_DEF */

