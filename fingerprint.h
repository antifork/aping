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

 typedef struct
   {
     unsigned long hash;
     char *        dscr;
   } fprint;

fprint fingerprint_vector[] = {
{ 1134562324U, "hping2 release"       },
{ 3259826144U, "BSD, linux 2.2"       },
{ 3904731629U, "openbsd, netbsd, aix" },  
{ 2384573344U, "linux 2.4a"           },
{ 3199159776U, "linux 2.4b"           },
{ 3710058560U, "linux 2.4c"           },
{  564517021U, "win98,me"	      },
{ 1463030269U, "win98 fe"             },
{ 3158421421U, "solaris 8"	      },
{  801825629U, "nt4"                  },
{ 2110941629U, "winxp,2k"             },
{ 2323906976U, "linux 2.2 PPC"	      },
{ 3219087789U, "netbsd PPC"           },
{ 2323906976U, "freebsd+ppp"	      },
{  833495333U, "router cisco"         },
{ 0          ,  NULL 		      }};

