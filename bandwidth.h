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

/* bandwidth dissect vector */
/* kbit/sec , descrition    */

#ifndef BANDWIDTH_H
#define BANDWIDTH_H

static
flink link_vector[] = {
{         0, "."                     },
{        28, "POTS 28.8/57k"         },
{        64, "ISDN 1B+D 64k"         },
{       128, "ISDN 2B+D 128k"        },
{       256, "CDN/FR 256k"           },
{       512, "CDN/FR 512k"           },
{      1544, "t1 (1.544 mbit)"       },
{      3088, "2-t1 (3.08 mbit)"      },
{      6176, "4-t1 (6.17 mbit)"      },
{     10000, "10baseT (8-t1)"        },
{     12352, "8-t1 (12.35 mbit)"     },
{     44736, "t3 (44.73 mbit)"       },
{     51840, "oc-1 (51.84 mbit)"     },
{     89472, "2-t3 (89.47 mbit)"     },
{    100000, "100baseT (oc-3)"       },
{    155520, "oc-3 (155.52 mbit)"    },
{    178944, "4-t3 (178.94 mbit)"    },
{    357888, "8-t3 (357.88 mbit)"    },
{    466560, "oc-9 (466.56 mbit)"    },
{    622080, "oc-12 (622.08 mbit)"   },
{    933120, "oc-18 (933.12 mbit)"   },
{   1244160, "oc-24 (1.244 gbit)"    },
{   1866240, "oc-36 (1.866 gbit)"    },
{   2488320, "oc-48 (2.488 gbit)"    },
{   4976640, "oc-96 (4.976 gbit)"    },
{   9953280, "oc-192 (9.953 gbit)"   },
{  13271040, "oc-256 (13.271 gbit)"  },
{ 999999999, NULL                   }};

#endif /* BANDWIDTH_H */
