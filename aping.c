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

#include "header.h"
#include "aping.h"

#include "typedef.h"
#include "prototype.h"

#include "macro.h"
#include "statistic.h"
#include "version.h"


/* global var */

#define  GLOBAL_HERE
#include "global.h"
#include "vectors.h"
#include "maturity.h"

#define ID "$Id$";

extern char pcap_version[];

char icon_set[]="->DZL>";

__inline void
print_icon(int t)
{
  printf("\r%c", icon_set[!t*(n_sent&1)+t]);
  fflush(stdout);
}


void
default_value ()
{
   int i;   

   rtt_max   = 0;
   rtt_min   = 9999;
   rtt_mean  = 0;
   rtt_sqre  = 0;

   tau       = 1000;
   ip_ttl    = 255;
   ip_tos    = 0;
   icmp_type = 8;
   icmp_code = 0;

   pattern   = (char *) malloc(49);

   for (i=0;i<48;i++) 
	pattern[i]=i+8; /* Berkeley style */	

   pattern[48]= 0; 

}


void
ctrl_ (i)
  int  i;
{
    iddeal++;
    iddeal&=3;

    out_burst  =0;
    max_burst  =0;
    rand_ip_id =0;
    fail_ip_id =0;
    slow_start =1;

    LOW_PASS_FIR(-1,0); /* reset low pass filter */

    PUTS ("idbug: %ld, differ: %ld\n\n",(iddeal &2)>>1, iddeal &1);

    options.differ   =  iddeal &1;
    signal (SIGQUIT, ctrl_);
}

void
ctrlz (i)
     int  i;
{
    
    if ( options.differ )
	{

           out_burst  =0;
           max_burst  =0;

	   traffic_tos= (traffic_tos+1) & 0x0f;
	
	   PUTS("\n%s: avrg size %ld bytes\n\n",
	   tcpip_lenght[traffic_tos].type,tcpip_lenght[traffic_tos].lenght);
	}
    else
	{
    	   verbose++;
    	   verbose&=3;

    	   PUTS ("verbose: %ld\n", verbose);
	}

    signal (SIGTSTP, ctrlz);
}

void
ctrlc (i)
     int  i;
{
    if ( ! options.sniff )
      { 
        if( pthread_cancel(pd_snd)!= 0 )
		{
		FATAL("pthread_cancel(): %s",strerror(errno));
		}
      }

    else
      {
        PUTS("done.\n");
	exit(1);
      }
}

int
main (argc, argv)
     int           argc;
     char        **argv;
{
    
    int           loss;
    int           es;
    int		  major;
    int 	  minor;

    /* maturity interface */

    SETUP_MATURITY();

    LOAD_MATURITY();

    /* Security */

    if (getuid() != geteuid())
        FATAL("aping doesn't run with +s bit set");

    if (getuid() != 0)
        FATAL("aping must run as root");

    if (argc < 2)
	FATAL ("no arguments given");
     
    /* refresh salt */
    srand (time (NULL));

    /* default */
    default_value ();

    while ((es = getopt (argc, argv, "S:D:O:T:MRI:t:k:i:c:p:e:Ps:z:fdnrvhb")) != EOF)
	switch (es)
	    {
	     case 'S':
		 SET (ip_src);
		 host_src = strdup (optarg);
		 break;

	     case 'D':
		 SET (ip_dst);
		 host_dst = strdup (optarg);
		 break;

	     case 'O':
		 if (!strcmp ("help", optarg))
		     usage (optarg, USAGE_TOS);

		 SET (ip_tos);
		 ip_tos = ATOA (optarg);
		 break;

	     case 'T':
		 SET (ip_ttl);
		 ip_ttl = ATOA (optarg);
		 break;

	     case 'M':
		 SET (ip_ramp);
		 break;

             case 'R':
                 SET (opt_rroute);
                 break;

	     case 'I':
		 if ( argv[optind] == NULL && *optarg == 'd' )
		 FATAL ("option requires an argument -Id int");
 
		 switch (*optarg)
		     {
		      case 'd':
			  SET (ip_id);
			  ip_id = ATOA (argv[optind++]);
			  break;
		      case 'i':
			  SET (ip_id_incr);
			  break;
		      case 'r':
			  SET (ip_id_rand);
			  break;
		      default:
			  FATAL ("invalid option -I%c", *optarg);
			  break;
		     }
		 break;

	     case 'b':
		usage (optarg, USAGE_BANDWIDTH);
		break;

	     case 't':
                 if (!strcmp ("help", optarg))
                     usage (optarg, USAGE_ICMP_TYPE);

		 SET (icmp_type);
		 icmp_type = ATOA (optarg);
		 break;

	     case 'k':
                 if (!strcmp ("help", optarg))
                     usage (optarg, USAGE_ICMP_CODE);

		 SET (icmp_code);
		 icmp_code = ATOA (optarg);
		 break;

	     case 'i':
		 SET (wait);
		 tau = ATOA (optarg);
		 break;
	     case 'c':
		 SET (count);
		 count = ATOA (optarg);
		 break;
	     case 'p':
		 SET (pattern);
                 free(pattern);
		 pattern = strdup (optarg);
		 break;
	     case 'e':
		 SET (ifname);
		 strncpy (ifname,optarg,16);
		 break;
	     case 'P':
		 SET (promisc);
		 break;
	     case 's':
		 SET (sniff);
		 verbose = ATOA (optarg);
		 break;
	     case 'z':
		 SET (size);
		 mysize = ATOA (optarg);
		 break;
	     case 'd':
		 SET (dfrag);
		 break;
	     case 'n':
		 SET (numeric);
		 break;
	     case 'r':
		 SET (droute);
		 break;
	     case 'f':
		 SET (differ);
		 break;
	     case 'v':
	   	 sscanf (pcap_version, "%d.%d", &major, &minor);
		 PUTS("aping %s (pcap version %d.%d)\n",VERSION,major,minor);
		 exit (1);
		 break;
	     case '?':
		 exit (1);
                 break;
	     case 'h':
		 usage (argv[0], USAGE_CLASSIC);
		 break;
	    }

            argc -= optind;
            argv += optind;

    ATOMIC_TEST(sniff,ip_src);
    ATOMIC_TEST(sniff,ip_dst);

    ATOMIC_TEST(ip_id_rand, ip_id_incr);
   
    DEPEND_TEST(sniff,  ifname);
    DEPEND_TEST(ip_src, ifname);

    if ( options.ip_src )
        ip_src = gethostbyname_lru (host_src);

    if ( options.ip_dst)
        ip_dst = gethostbyname_lru (host_dst);

    /* destination.. */

    if (*argv != NULL)
	{
    	   host_dst = strdup(*argv);
    	   ip_dst   = gethostbyname_lru (*argv);
 	}
    else
    if (!options.sniff)	
	FATAL("no destination given");

    
    if ( !options.ip_src ) 
	{

    	   if ( !options.ifname )
        	get_first_hop (ip_dst, &ip_src, ifname);
    	   else
        	ip_src = gethostbyif(ifname);

	}

    /* Catch signal */
    signal (SIGINT,  ctrlc);
    signal (SIGTSTP, ctrlz);
    signal (SIGQUIT, ctrl_);


     /* set myid */
     myid = getpid () & 0xffff;   

     pthread_mutex_init(&pd_mutex, NULL); /* mutex setup */
     pthread_create    (&pd_rcv, NULL, (void *) receiver, NULL);

    if ( ! options.sniff )
        {
	   pthread_create(&pd_snd, NULL, (void *) sender, argv );

	   /* waiting for pd_snd exit/cancel */
	   pthread_join  (pd_snd, NULL);
	}
    else 
           pthread_join  (pd_rcv, NULL);	

     if (n_sent)
        {
	  int wait_time;
         
          wait_time = ( n_tome ? \
			rtt_mean+100+2*ISQRT(rtt_sqre-rtt_mean*rtt_mean) : 2000 );
 
          PUTS("--- sleeping %d ms (RTT+2 sigma) ---\n", wait_time );

	  usleep(wait_time* 1000);
          pthread_cancel(pd_rcv);
          loss = 100 - PER_CENT (n_tome, n_sent);

          PUTS ("\n--- %s aping statistics ---\n", multi_inet_ntoa (ip_dst));
          PUTS ("%ld packets transmitted, %ld packets received, %d %% packets loss\n", n_sent, n_tome, loss);

          if (n_tome)
          PUTS ("round-trip min/mean/dstd/max = %ld/%ld/%ld/%ld ms\n",
		rtt_min,rtt_mean,ISQRT(rtt_sqre-rtt_mean*rtt_mean ),rtt_max);
        }

     else
        PUTS ("done.\n");

     exit(1);
}
