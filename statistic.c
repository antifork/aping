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

#include "typedef.h"
#include "prototype.h"
#include "global.h"

#include "filter.h"
#include "bandwidth.h"
#include "statistic.h"


char         *
link_type (long kb)
{
    int           i;

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
#define P_MASK    (~(P_SIZE-1))   /* 11111111111111111111000000000000 */
#define K_MASK    (~(K_SIZE-1))   /* 11111111111111111100000000000000 */

#define IND(x)    ((int)x>>2 & I_MASK)

long
E (long * addr, long data, long n)
{
    static long long sum[2][P_SIZE];
    static long      cnt[2][P_SIZE];
           long      counter;

    if ( n != 0 && n != 1 && n != 2 )
        {
        fprintf(stderr,"Expectation error: E{x^n} ? n=1 | n=2\n");
        exit(-1);
        }

    if ( n == 0 )
        {
          sum[n-1][IND(addr)] = 0;
          cnt[n-1][IND(addr)] = 0;
	  *addr               = 0;
	  return 0;
        }

    if ( sum[n-1][IND(addr)] != 0 && ( cnt[n-1][IND(addr)] & K_MASK ) != ( (long)addr & K_MASK ) )
        {
        fprintf(stderr,"Expectation error: data collision\n");
        exit(-1);
        }

    counter = cnt[n-1][IND(addr)] & ~K_MASK;
    counter++;

    if ( counter >= K_SIZE )
                {
                fprintf(stderr, "Expectation error: memory overflow\n");
                sum[n-1][IND(addr)]=0;
                cnt[n-1][IND(addr)]=0;
                *addr              =0;
                return -1;
                }

    switch(n)
        {
        case 1:
                sum[n-1][IND(addr)]+= data;
                break;
        case 2:
                sum[n-1][IND(addr)]+= data*data;
                break;
        }

    cnt[n-1][IND(addr)] = ( ((long)addr & K_MASK) | (counter & ~K_MASK) );
    *addr               = sum[n-1][IND(addr)]/counter;

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

    do
	{
	    c += i & 1;
	    j <<= 1;
	}
    while (i >>= 1);

    if (c == 1)
	return x;
    else
	return (j ? j : 0xff);

}

/*
 * filter 
 */

#define C0 100
#define C1 36
#define C2 13
#define C3 4

#define FIR(a,b,c,d) (a* C0+b* C1+c* C2+d* C3)/(C0+C1+C2+C3)

double
single_step_FIR (double n)
{
    static double enne[3];

    double        ret;

    if (n < 0)
	{
	    /* reset */

	    enne[0] = 0;
	    enne[1] = 0;
	    enne[2] = 0;

	    return 0;
	}

    ret = FIR (n, enne[0], enne[1], enne[2]);

    enne[2] = enne[1];
    enne[1] = enne[0];
    enne[0] = n;

    return ret;

}


long
LOW_PASS_FIR (long bit, long pt)
{
    static long   mean_pt;

    double        ret;

    long          n_step;
    long          s_step;


    if (bit < 0)
	{			/* reset */
	    mean_pt = 0;
	    return single_step_FIR (bit);
	}

    ret = single_step_FIR (bit / pt);

    if (!mean_pt)
	mean_pt = pt;

    n_step = pt / mean_pt;	/* number of single step to perform for the current burst */

    if (n_step > 1)
	{

	    for (s_step = 0; s_step < (n_step - 1); s_step++)
		ret = single_step_FIR (bit / pt);
	}

    mean_pt = (mean_pt + pt) >> 1;

    return (long) ret;

}


long
magic_round(long a, long b)
{
  int s = a;

  while ( s <= b ) s+=a;

  if ( ABS (s-b) < ABS(s-a-b) )
        return s;
  else
        return s-a;

}


long          delta;

void
bandwidth_predictor (packet * p)
{

    long  pending_time;
    long  local_tau;

    /*

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
  tau        |       |`--.             |     } 
   |         |       |    `--.         |     }
   |         |       |        `-       |     }
   |         |       |                 |     }
   |     time_lost   |                 |     }
   |                 |`--.             |     }
   |         |       |    `--.         |     } pen_time =(time_lost+last_rtt)+
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
     */


    local_tau    = last_rtt.ms_int + time_lost;
    local_tau    = magic_round (tau, local_tau);
 
    pending_time = MAX(tau/2, local_tau+ jitter/2 );   /* <- min of pending time is TAU/2 */

    if (options.differ & !rand_ip_id)
	{

	    delta = (diff_id > 1 ? diff_id - 1 : 0);

	    /* a low_pass filter voids spikes */

	    out_burst = LOW_PASS_FIR (delta * (tcpip_lenght[traffic_tos].lenght << 3), pending_time);
	    max_burst = MAX (max_burst, out_burst);
	    
	    E ( &mean_burst, out_burst, 1 );

	    PUTS ("    burst=%ld mean_burst=%ld max_burst=%ld kbps ",out_burst,mean_burst,max_burst);
	    PUTS ("link=[%s]\n", link_type(max_burst));

	}
    else if (rand_ip_id)
	{
	    /* rand_ip_id detected!!! OPENBSD| FREEBSD */
	    PUTS ("    burst= RAND_IPID(+%ld)\n", rand_ip_id);
	}


}
