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

#ifndef MATURITY_H
#define MATURITY_H

/* Maturity level symbols:

   _ : not implemented
   i : incomplete
   a : alpha   module
   b : beta    module
   * : working module 
*/

#define IF_LOADER(x)  if ( icmp_loader_vector[x] != NULL )
#define IF_DISSECT(x) if ( icmp_dissect_vector[x] != NULL )


#define SETUP_MATURITY() 						\
{									\
int _i;									\
for (_i=0;_i<64;_i++)							\
  {									\
   maturity_level[_i][0]=' ';						\
   maturity_level[_i][1]=' ';						\
  } 									\
}

#define LOAD_MATURITY()                         				\
{                                               				\
int _i;                                         				\
for (_i=0;_i<64;_i++)                          					\
  {                                             				\
   IF_LOADER(_i) {(*icmp_loader_vector [_i]) ((packet *)maturity_void, NULL);} 	\
   IF_DISSECT(_i){(*icmp_dissect_vector[_i]) ((packet *)maturity_void);} 	\
  }                                             				\
}

#define SET_LOADER_LEVEL(c)					\
{								\
 if ( p == (packet *)maturity_void )  				\
    {								\
      maturity_level[_dissect_type][0]=(c);			\
      return;							\
    }								\
}

#define SET_DISSECT_LEVEL(c)					\
{                                               		\
 if ( p == (packet *)maturity_void )           			\
   {                                         			\
     maturity_level[_dissect_type][1]=(c);			\
     return;                                 			\
   }                                         			\
}                           

#endif /* MATURITY_H */
