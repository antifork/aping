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

    if ( n != -1 && n != -2 && n != 1 && n != 2 )
        {
        fprintf(stderr,"Expectation error: E{x^n} ? n=1 | n=2\n");
        exit(-1);
        }

    if ( n < 0 )
        {
          sum[-n-1][IND(addr)] = 0;
          cnt[-n-1][IND(addr)] = 0;
	  *addr                = 0;
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

/*****
 
    A linear bandwidth predictor: a stocastic model.  
              Bonelli Nicola <bonelli@blackhats.it>

    We model the bandwidth with the following law:

    raw(t) = b(t) + n(t)

    where:
                                                               ________
     raw = raw data read by receiver...  ( raw(t) = diff_id(t)*packsize/tau )

     b   = effective instantaneous bandwidth we want to predict 
     n   = noise due to the quantization.


     b(t) is a stocastic process whose autocorrelation function can be 
          thought like this:

     Rb(tau) = |1-tau/T| rect (tau/(2T))  ( triangolar autocorrelation )


          Rb(tau)            
            ^
            |
            |                                              ___                 
            ^           where T is the mean of rtt. (  T = tau )
           /|\
          / | \
         /  |  \
        /   |   \
     -------+--------> tau 
            |    T 

     This model is likehood, in the sense that for each couple of samples 
                      ___
     whose distance > tau the two are incorrelated.

     n(t) is a stocastic model, indipendent from the first one, whose IO 
          characteristic has the following graph:

            N                                        Fn
            ^                                        ^ 
            |                                        |
            |                                        |
            |  .     .     .                    +----+----+ 
            | /|    /|    /|                    |    |    |
            |/ |   / |   / |                    |    |    |
            +-----/-----/----->            -----+----+----+----->   
            |  | /   | /                             |    packsize/2
            |  |/    |/
            |  '     ' 



     The model we use for our 2 step linear predictor is the following:
     _          _
     b(t+T) = b(t)*Rb(T) + raw (t+T)* (1-Rb(T));        

    
     Implementation diagram: 


        id(t)

         |
         |                      .-------.       .-------.
          `--->(-)-----(*)----->| 2step |--+--->|  FIR  |---> burst(t)
                ^   |   ^       ._______.  |    ._______.
                |   |   |          ^       |
               [T]--'   |           `--[T]-'
                        |
                    _________
                    pack_size
                   

*****/

#define TRIANGLE(x) ( (x > tau) ? 0 : tau-(x) )

long
twostep_predictor(long raw, long pen_time )
{
  static long b_t;

  b_t  = b_t * TRIANGLE(pen_time) + (raw/(1+pen_time)) * \
		( tau - TRIANGLE(pen_time) );

  b_t /= tau;

  return (b_t);

}


/*
 *
 *  low pass filter: h(n)= 100 e^(-n*4/3)*{ u(n)-u(n-3) } 
 *                       = d(n)*100 + d(n-1)*26 + d(n-3)*6 
 *   
 */

#define h0 100    /* n=0 */ 
#define h1 26     /* n=1 */
#define h2 6      /* n=2 */

#define FIR(a,b,c) (a* h0+b* h1+c* h2)/(h0+h1+h2)

double
onestep_FIR (double n)
{ 
    static double enne[2];
  
    double        ret;
  
    if (n < 0)
        {
            /* reset */
       
            enne[0] = 0;
            enne[1] = 0;
       
            return 0;
        }
       
    ret = FIR (n, enne[0], enne[1]);
  
    enne[1] = enne[0];
    enne[0] = n;
  
    return ret;
  
}


long
lp_FIR (long kbps, long pt)
{ 

    double        ret;
    long          ns;
    long          i;
  
    if ( kbps < 0) /* reset */
        {
            return onestep_FIR (kbps);
        }

    ns =  pt / tau;          /* how many times the onestep_fir is called  
                              */
    if ( ns == 0 ) 
	ns=1;

    for ( i= 0; i < ns ; i++)
        ret = onestep_FIR (kbps);

    return (ret);

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


void
bandwidth_predictor (packet * p)
{

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
 local_tau   |       |`--.             |     } 
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


    if ( options.sniff )
	return;

    if ( curr_tstamp )
	{

	/* icmp timestamp reply: 
	 *  In case of ts_reply we dont need to estimate the pending_time, 
	 *  since we are able to calc a deterministic value.
	 */

	pending_time = MAX( 0 , curr_tstamp- last_tstamp);

	} 
    else
	{
    	
	local_tau    = last_rtt.ms_int + time_lost;
    	local_tau    = magic_round (tau, local_tau);

    	pending_time = MAX(tau/2, local_tau+ jitter/2 );   /* <- min of pending
							    *    time is TAU/2 
							    */

	}
	
    if (options.differ & !rand_ip_id)
	{

	  delta = (diff_id > 1 ? diff_id - 1 : 0);

	    /* a low_pass filter voids spikes */

	  out_burst = twostep_predictor (delta *(tcpip_lenght[traffic_tos].lenght << 3), pending_time);
	  out_burst = lp_FIR( out_burst , pending_time );

	  max_burst = MAX (max_burst, out_burst);

	  E ( &mean_burst, out_burst, 1 );

	  PUTS ("    burst=%ld mean_burst=%ld max_burst=%ld kbps ",out_burst,mean_burst,max_burst);
          PUTS ("usage=%d%%(%d%%) ",PER_CENT(out_burst,max_burst),PER_CENT(mean_burst,max_burst));
	  PUTS ("link=[%s/%s]\n", link_type(mean_burst),link_type(max_burst));

	}
    else if (rand_ip_id)
	{
	    /* rand_ip_id detected!!! OPENBSD| FREEBSD */
	    PUTS ("    burst+= rand(ip_id)\n");
	}


}
