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


#ifndef MACRO_H
#define MACRO_H

#define FATAL(f,arg...) {						\
                        fprintf(stderr,"%s:%d: ",__FILE__,__LINE__);	\
                        fprintf(stderr,f,## arg);			\
                        fputs (". exit-forced\n",stderr);		\
                        exit(1);					\
                        }

#define PUTS(f,arg...)	({						\
			fprintf(stderr,f,## arg);			\
			fflush (stderr);				\
			})

#ifdef  EADBUG
#define DEBUG(s,f,arg...) ({                                              \
                          fprintf(stderr,"%s::",s);                       \
                          fprintf(stderr,f,## arg);                       \
                          })
#else
#define DEBUG(s,f,arg...) {} 
#endif

#define DONT_EAT_CPU()  usleep(1);	

#define ATOA(x) 	strtol(x, (char **)NULL, 0)

#define ATOMIC(a,b)	( (!(a)&&(b)) || ((a)&&!(b)) || (!(a)&&!(b)) )

#define ATOMIC_TEST(a,b)                                \
{                                                       \
if ( !ATOMIC(options.##a,options.##b) )                 \
FATAL("set either of these options: %s | %s",#a,#b);    \
}

#define DEPEND_TEST(a,b)				\
{ 							\
if ( options.##a && !options.##b )			\
FATAL("%s option require %s",#a,#b);			\
} 

#define INDEX(a,b) ((a)*64+b)

#define SET(x) (options.##x=1 )
#define CLR(x) (options.##x=0 )
#define TST(x) (options.##x==1)

#define PER_CENT(a,b) ( (b) == 0 ? 0 : 100*(a)/(b) )

#define FREE(a) { if ( a != NULL ) free(a); }
 
#define STR(a)    # a
#define CAT(a,b)  a ## b
#define XCAT(a,b) CAT(a,b)
#define ENDLESS() for(;;)

#if defined(__FreeBSD__)
#define FIX(x)	 (x)
#define UNFIX(x) (x)
#else
#define FIX(x)	 htons(x)
#define UNFIX(x) ntohs(x)
#endif

#define HOP_DISTANCE(x)  TTL_PREDICTOR(x)-x

/* times */

#define DIFFTIME_int(a,b) (( CAT(a,_sec)  - CAT(b,_sec)  ) * 1000 + ( CAT(a,_usec) - CAT(b,_usec))/ 1000)
#define DIFFTIME_frc(a,b) (( CAT(a,_usec) - CAT(b,_usec) ) % 1000 )
#define TIME_ADJUST(i,f)   if ( (f) < 0 ) { (i)++; (f) += 1000; }

/* math */

#ifndef MIN
#define MIN(a,b) ((a)<(b) ? (a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b) ? (a):(b))
#endif

#define SIGN(x)  ( (x) > 0 ? 1  : ( (x) == 0 ?   0  :  -1  ) )
#define U(x)	 ( (x) > 0 ? x  : 0 )
#define ABS(x)   ( (x) < 0 ? -(x) : (x) )

#define ISQRT(v) ({             \
unsigned long g;                \
unsigned long bit;              \
g   = 0;                        \
bit = 1<<15;                    \
do {                            \
g ^= bit;                       \
if (g * g > v )                 \
g ^= bit;                       \
} while (bit >>=1);             \
g;                              \
})

#define PRINT_IPH_64bit(p)	{					\
if ( ICMP_IP_p(p) == IPPROTO_TCP || ICMP_IP_p(p) == IPPROTO_UDP )	\
   {									\
        								\
   PUTS("    proto[%s] ip_src=%s:%d -> ip_dst=%s:%d ",			\
             protocols[ICMP_IP_p(p)],					\
             multi_inet_ntoa(ICMP_IP_src(p)),				\
             ntohs(ICMP_TCP_sport(p)),					\
             multi_inet_ntoa(ICMP_IP_dst(p)),				\
             ntohs(ICMP_TCP_dport(p)) );				\
   }									\
else									\
   {									\
   PUTS("    proto[%s] ip_src=%s -> ip_dst=%s ",			\
             protocols[ICMP_IP_p(p)],					\
             multi_inet_ntoa(ICMP_IP_src(p)),				\
             multi_inet_ntoa(ICMP_IP_dst(p)) );				\
   }									\
}

#endif /* MACRO_H */
