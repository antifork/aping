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


#include "dissect.h"
#include "aping.h"

#include "header.h"
#include "typedef.h"
#include "prototype.h"

#include "filter.h"
#include "bandwidth.h"
#include "statistic.h"
#include "global.h"

char *
link_type (long kb)
{
    int i;

    i = 0;

    /*  ( base+ base 12.5%)  < kbit ? */

    while ((link_vector[i].kbit + (link_vector[i].kbit >> 3)) < kb)
	i++;

    return link_vector[i].type;

}


/* Operator E */

/*
 *                    KEY                           INDEX       
 *                   18 bit                        12 bit 
 *     _________________________________ ________________________
 *    '                                 `'                       `
 *    _______________ _________________ _____________ _______________  
 *   | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |x|x|
 *    7             0 7             0 7             0 7             0 
 *    `--------------------------------------'`----------------------'
 *                   20 bit PAGE                   12 bit INDEX
 *
 */

#define P_BIT     12
#define K_BIT     14
#define P_SIZE    (1UL << P_BIT)
#define K_SIZE    (1UL << K_BIT)
#define I_MASK    (P_SIZE-1)
#define P_MASK    (~(P_SIZE-1))	/* 11111111111111111111000000000000 */
#define K_MASK    (~(K_SIZE-1))	/* 11111111111111111100000000000000 */

#define IND(x)    ((int)x>>2 & I_MASK)

long
E (long *addr, long data, long n)
{
    static long long sum[2][P_SIZE];
    static long cnt[2][P_SIZE];
    long counter;

    if (n != -1 && n != -2 && n != 1 && n != 2) {
	FATAL ("Expectation error: E{x^n} ? n=1 | n=2\n");
    }

    if (n < 0) {
	sum[-n - 1][IND (addr)] = 0;
	cnt[-n - 1][IND (addr)] = 0;
	*addr = 0;
	return 0;
    }

    if (sum[n - 1][IND (addr)] != 0 && (cnt[n - 1][IND (addr)] & K_MASK) != ((long) addr & K_MASK)) {
	FATAL ("Expectation error: data collision\n");
    }

    counter = cnt[n - 1][IND (addr)] & ~K_MASK;
    counter++;

    if (counter >= K_SIZE) {
	fprintf (stderr, "Expectation error: memory overflow\n");
	sum[n - 1][IND (addr)] = 0;
	cnt[n - 1][IND (addr)] = 0;
	*addr = 0;
	return -1;
    }

    switch (n) {
     case 1:
	 sum[n - 1][IND (addr)] += data;
	 break;
     case 2:
	 sum[n - 1][IND (addr)] += data * data;
	 break;
    }

    cnt[n - 1][IND (addr)] = (((long) addr & K_MASK) | (counter & ~K_MASK));
    *addr = sum[n - 1][IND (addr)] / counter;

    return 0;
}



#define HOP_DISTANCE(x)  TTL_PREDICTOR(x)-x

unsigned char
TTL_PREDICTOR (unsigned char x)
{
    register unsigned char i;
    register unsigned char j;
    register unsigned char c;

    i = x;
    j = 1;
    c = 0;

    do {
	c += i & 1;
	j <<= 1;
    }
    while (i >>= 1);

    if (c == 1)
	return x;
    else
	return (j ? j : 0xff);

}


long
magic_round (long a, long b)
{
    int s = a;

    while (s <= b)
	s += a;

    if (ABS (s - b) < ABS (s - a - b))
	return s;
    else
	return s - a;

}


/*****
    
     Implementation diagram: 


        id(t)

         |
         |                        .-------.
          `--->(-)-----(*)------->|  FIR  |---> burst(t)
                ^   |   ^         ._______.
                |   |   |     
               [T]--'   |    
                        |
                    _________
                    pack_size
                   

     Temporal diagram:


                   APING           REMOTE_HOST 
                     |                 |
  __________________ |                 |
   ^         ^       |`--.             |
   |         |       |    `--.         |
   |         |       |        `--.     |
   |     last_rtt    |            `--. |
   |                 |                >| <--.
   |         |       |            ,--' |     }
   |         v       |        ,--'     |     }
   |   _____________ |    ,--'         |     }
   |         ^       |,--'             |     }
 real_tau    |       |`--.             |     } 
   |         |       |    `--.         |     }
   |         |       |        `-       |     }
   |         |       |                 |     }
   |  waiting_time   |                 |     }
   |                 |`--.             |     }
   |         |       |    `--.         |     } pen_time =(waiting_time+last_rtt)+
   |         |       |                 |     }            jitter/2;
   |         |       |                 |     }
   |         |       |                 |     }
   v         v       |                 |     }
  ___________________|                 |____ }_______
           last_sent>|`-.              |     }
                     |   `-.           |     }    ^    
                     |      `-.        |     }    |
                     |         `-.     |     }    |
                     |            `-.  |     }    |
                     |               `>| <--'     |    
                     |             .-' |         rtt   jitter = rtt - last_rtt 
                     |          .-'    |          |
                     |       .-'       |          |
                     |    .-'          |          |
                     | .-'             |          v
                     |-                |_____________



*****/


void
bandwidth_predictor (packet * p)
{

    if (options.sniff || !diff_ipid)
	return;

    if (rand_ipid != 0) {
	/* rand_ipid ... OpenBSD|FreeBSD */
	PUTS ("    burst+= rand(ip_id)\n");
	return;
    }

    if (curr_tstamp) {

	/*  In case of ts_reply we dont need to estimate the pending_time, 
	 *  since it is possibile to calculate the deterministic value.
	 */

	pending_time = MAX (0, curr_tstamp - last_tstamp);

    }
    else {

	/*  pending_time estimate: MAX (tau/2, real_tau + jitter/2 )
         */
         
	real_tau = last_rtt.ms_int + waiting_time;
	real_tau = magic_round (tau, real_tau);

	pending_time = MAX (tau/2, real_tau + jitter/2);

    }

    out_burst  = (MAX (0, diff_id-1 ) * tcpip_lenght[traffic_tos].lenght <<3)  + (ip_size<<3); 	/* bits */ 
    out_burst /= pending_time; 									/* kbps */

    max_burst = MAX (max_burst, out_burst);

    E (&mean_burst, out_burst, 1);

    PUTS ("    burst=%ld mean_burst=%ld max_burst=%ld kbps ", out_burst, mean_burst, max_burst);
    PUTS ("link=[%s]\n", link_type (mean_burst));

}
