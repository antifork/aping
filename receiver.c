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

#include "global.h"
#include "def_icmp.h"

int icmp_parent[32] = { 8, -1, -1, -1, -1, -1, -1, -1, 0, -1, -1, -1, -1, 0, 13, 0, 15, 0, 17, -1, -1 };


struct pcap_pkthdr pcaphdr;
struct bpf_program fcode;
struct timeval *timestamp;
packet        pkt;

#include "vectors.h"
#include "filter.h"

void
lineup_layers (char *buff, packet * pkt)
{

  pkt->ip = (struct ip *) ((void *) buff + offset_dl);
  pkt->icmp = (struct icmp *) ((void *) buff + offset_dl + (pkt->ip->ip_hl << 2));
  pkt->data = (char *) ((void *) buff + offset_dl + (pkt->ip->ip_hl << 2) + 8);

  pkt->icmp_tstamp_tval = (struct timeval *) ((void *) buff + offset_dl + (pkt->ip->ip_hl << 2) + 8); /* icmp_type 0|8 */
  pkt->icmp_tstamp_data = (char *) ((void *) buff + offset_dl + (pkt->ip->ip_hl << 2) + 8 + sizeof (struct timeval));

}

unsigned short
ntohss (s)
     unsigned short s;
{
    unsigned short ret;

    ret = 0;

    if (iddeal & 2)
	{
	    ret = s;
	}
    else
	{
	    ret |= (s >> 8);
	    ret |= (s << 8);
	}

    return ret;
}


#ifdef __GNUC__
__inline
#endif
    void
agent_timestamp (packet * p)
{

    switch( ICMP_type (p) )
	{

	case ICMP_ECHOREPLY: 

		rtt.ms_int = DIFFTIME_int (timestamp->tv, TVAL_tv(p));
		rtt.ms_frc = DIFFTIME_frc (timestamp->tv, TVAL_tv(p));

		break;

	case ICMP_TSTAMPREPLY: /* timestamp reply */

		rtt.ms_int  = (timestamp->tv_sec %(24*60*60))*1000 +timestamp->tv_usec/1000 - ntohl(ICMP_otime(p)); 
		rtt.ms_frc  = 0;

		curr_tstamp = ntohl(ICMP_rtime(p));

		break;

	default: /* stimated rtt */

	    	rtt.ms_int = DIFFTIME_int(timestamp->tv,last_sent.ts);
            	rtt.ms_frc = DIFFTIME_frc(timestamp->tv,last_sent.ts);
		break;

	}


    TIME_ADJUST(rtt.ms_int,rtt.ms_frc);

    rtt.ms_int = ABS (rtt.ms_int);

    rtt_max    = MAX (rtt_max, rtt.ms_int + 1);
    rtt_min    = MIN (rtt_min, rtt.ms_int);

    E (&rtt_mean,rtt.ms_int, 1);
    E (&rtt_sqre,rtt.ms_int, 2);

    jitter     = rtt.ms_int - last_rtt.ms_int;


    return;
}

#ifdef __GNUC__
__inline
#endif
    int
agent_ipid (packet * p)
{
    if (diff_id && !(diff_id & 0xff))
	{
	    /* wrong id-endian */

	    fail_ip_id++;

	    switch (fail_ip_id)
		{
		 case 1:
		 case 2:
		     break;
		 case 3:
		 case 4:
		 case 5:
		     PUTS ("\n!!!WARNING: wrong ip_id endianess!!!");
		     return -1;
		     break;
		 case 6:
		     PUTS ("\n!!!REVERTING to correct endian!!! ");

		     fail_ip_id = 0;
		     iddeal    ^= 2;

		     out_burst  = 0;
		     max_burst  = 0;

		     rand_ip_id = 0;
		     fail_ip_id = 0;

		     slow_start = 1;	/* prevent wild burst */

		     LOW_PASS_FIR (-1, 0);	/* reset low pass filter */

		     PUTS ("idbug: %ld, differ: %ld\n", (iddeal & 2) >> 1, iddeal & 1);
		     return -1;

		     break;

		}
	}

    else
	fail_ip_id = 0;

    return 0;
}



#ifdef __GNUC__
__inline
#endif
    void
agent_dyn_id (packet * p)
{

    curr_id = ntohss (IP_id (p));

    if (slow_start)
	{
	    diff_id = 0;
	    slow_start = 0;
	}
    else
	diff_id = curr_id - last_id;

    if (diff_id < 0)
	diff_id += (1 << 16) - 1;

    if (diff_id > 1 << 15)
	rand_ip_id = diff_id;

    return;
}


void
print_RR(char *opt)
{
  long *hop;
  long  rr;

  int   i;

   rr = hash(opt, 40); 
  
   if ( last_route != rr )
	{
	/* new RR */

	 last_route = rr;

	 hop = (long *)(opt+3); /* hop pointer */ 


	 for(i=1; i < (opt[IPOPT_OFFSET]>>2); i++ ) 	
		{
		PUTS("\rRR:  %s(%s)\n", gethostbyaddr_lru(hop[i-1]),multi_inet_ntoa(hop[i-1]));	
		}


	}
}


void
process_pack (packet * p)
{
    char         *saddr;
    char         *daddr;


    saddr = gethostbyaddr_lru (IP_src (p));
    daddr = gethostbyaddr_lru (IP_dst (p));


    /* print record_route if new */

    if ( options.opt_rroute )
    	{
	print_RR(IP_opt(p)); 
    	}	

    /* dup | zombie  */

    if (ICMP_HAS_SEQ (p))
	{

	    if ((ICMP_seq (p) - last_seq) == 0)
		{
	            print_icon(ICON_DUP);
		    goto end_switch;
		}

	    if ( ZOMBIE(p) )
		{
		    print_icon(ICON_ZOMBIE);
		    goto end_switch;
		}

	  print_icon(ICON_OK);
	  end_switch:

	}
	else
	print_icon(ICON_OK);

    /* bytes */

    PUTS ("%db", ntohs (IP_len (p)) - (IP_hl (p) << 2));


    /* from | from to */

    PUTS (" from %s", saddr);

    if (options.sniff)
	{
	    PUTS (" -> to %s:", daddr);
	}
    else
	{
	    PUTS (":");
	}

    /*** icmp layer ***/

    /* type */

    PUTS (" icmp=%d(%s)", ICMP_type (p), icmp_type_str[ICMP_type (p) & 0xff]);

    /* code */

    if (icmp_code_str[((ICMP_type (p) & 0xff) << 5) + (ICMP_code (p) & 0xff)] != NULL)
	{
	    PUTS (" code=%d(%s)", ICMP_code (p), icmp_code_str[((ICMP_type (p) & 0xff) << 5) + (ICMP_code (p) & 0xff)]);
	}
    else
	{
	    if (ICMP_code (p) != 0)
		PUTS (" code=%d(?)", ICMP_code (p));
	}

    /* icmp seq */

    if (ICMP_HAS_SEQ (p))
	PUTS (" seq=%d", ICMP_seq (p));

    /* icmp timestamp diff: echo reply */

    if (!options.sniff && (ICMP_type (p) == 0 || ICMP_type(p) == 14 ) )
	{

	    PUTS (" rtt=%ld.%ld ms", rtt.ms_int, rtt.ms_frc);
	}

    /* id_ip (IP layer) */

    if (options.differ)
        {
            PUTS (" ip_id+=%ld\n", diff_id);
        }
    else
        {
            PUTS (" ip_id=%ld\n", curr_id);
        }

    if (verbose < 1)
	goto end;

    /* verbose == 1 */

    PUTS ("    ttl=%-3d dst=%d(hop)", IP_ttl (p), HOP_DISTANCE (IP_ttl (p)));

    if (ICMP_HAS_ID (p))
	PUTS (" icmp_id=%d", ICMP_id (p));
 
    PUTS (" jitter=%ld ms time_lost=%ld ms\n", jitter, time_lost); 

    /* id_ip (IP layer) */

    if (verbose < 2)
	goto end;

    /* verbose == 2 */

    if (icmp_dissect_vector[ICMP_type (p) & 0xff] != NULL)
	{
	    (*icmp_dissect_vector[ICMP_type (p) & 0xff]) (p);
	}

  end:

    return;

}


void
receiver ()
{
    packet       *p;
    sigset_t      set;
    pcap_t       *in_pcap;

    const u_char *ptr;

    int           fval;
    int           pcap_fd;


    /* setup */

    last_seq = -1;
    last_id = -1;

    fval = 0;
    curr_id = 0;
    diff_id = 0;

    last_rtt.ms_int = 0;
    last_rtt.ms_frc = 0;

    rtt.ms_int = 0;
    rtt.ms_frc = 0;

    slow_start = 1;

    sigemptyset (&set);

    /* masquerading signal */

    sigaddset (&set, SIGTSTP);
    sigaddset (&set, SIGINT);
    sigaddset (&set, SIGQUIT);

    pthread_sigmask (SIG_BLOCK, &set, NULL);

    pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    p = &pkt;

#if defined(__FreeBSD__)
    if ((in_pcap = pcap_open_live (ifname, 1024, options.promisc, 10, bufferr)) == NULL)
#else
    if ((in_pcap = pcap_open_live (ifname, 1024, options.promisc, 0, bufferr)) == NULL)
#endif
	FATAL (bufferr);

    pcap_fd = pcap_fileno (in_pcap);

#ifndef __FreeBSD__
    fcntl (pcap_fd, F_GETFL, fval);
    fval |= O_NONBLOCK;
    fcntl (pcap_fd, F_SETFL, fval);
#endif
    /* set filter */

    if (pcap_lookupnet (ifname, &localnet, &netmask, bufferr) == -1)
	FATAL (bufferr);

    if (pcap_compile (in_pcap, &fcode, "icmp", 0, netmask) == -1)
	FATAL ("pcap compile: %s", strerror (errno));

    if (pcap_setfilter (in_pcap, &fcode) == -1)
	FATAL ("pcap_setfilter: %s", strerror (errno));

    /* set offset_dl: datalink header size */

    if (sizeof_datalink (in_pcap) == -1)
	exit (-1);

    PUTS ("%s[%s] (%s/%s) with %ld bytes of datalink layer.\n", ifname, \
	multi_inet_ntoa ((long) ip_src), multi_inet_ntoa ((long) localnet), \
	multi_inet_ntoa ((long) netmask), offset_dl);

    ENDLESS ()
    {

	DONT_EAT_CPU();

	ptr = NULL;

	while ((ptr = pcap_next (in_pcap, &pcaphdr)) != (u_char *)NULL)
	    {
	
		/* timestamp of current packet */

		timestamp = &(pcaphdr.ts); 

		lineup_layers ((char *)ptr, p);

 		n_recv++;


		if (GENERIC_FILTER (p))
		    {

                	if (REPLY_FILTER (p))
                    	n_tome++;

			switch( ICMP_type(p))
				{

				case ICMP_ECHOREPLY:
					time_lost =   DIFFTIME_int (TVAL_tv(p),last_ack.ts);
					break;

				default:
					time_lost =   DIFFTIME_int (last_sent.ts, last_ack.ts );
					break;

				}

			last_ack.ts_sec  = timestamp->tv_sec;
			last_ack.ts_usec = timestamp->tv_usec;

			agent_timestamp (p);
			agent_dyn_id (p);

			if (agent_ipid (p) != -1)
			   {	
				process_pack (p);
			   }


			if (ICMP_HAS_SEQ (p))
			    last_seq = ICMP_seq (p);

			last_id         = curr_id;
			last_rtt.ms_int = rtt.ms_int;

			last_tstamp     = curr_tstamp;

		    }
	    }
      }
}
