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
#include "version.h"

#include "bandwidth.h"

#include "global.h"

extern char  *icmp_type_str[64];
extern char  *icmp_code_str[64 * 64];

void
usage (char *name, int type)
{

    switch (type)
	{

	 case USAGE_CLASSIC:
	     fprintf (stderr,
		      "usage: %s [options] host args...\n"
		      " Layer ip\n"
		      "   -S  ip                    src ip\n"
		      "   -D  ip                    dst ip\n"
		      "   -O  tos                 * set tos\n"
		      "   -T  ttl                   set time to live\n"
		      "   -M                        set TTL++ modulation\n"
		      "   -Id id                    set base ip_id\n"
		      "   -Ii                       set incremental ip_id\n"
		      "   -Ir                       set random ip_id\n"
		      "   -R                        set record route ip_opt\n"
		      " Layer icmp\n"
		      "   -t  type args...        * set icmp type\n"
		      "   -k  code                * set icmp code\n"
		      " Others\n"
		      "   -i  msec                  set wait msec (default 1000)\n"
		      "   -p                        set pattern \"\"\n"
		      "   -e  ifname                set default device\n"
		      "   -P                        set promisc-mode\n"
		      "   -s  level                 set sniff level\n"
		      "   -z  size                  set packetsize\n"
		      "   -c                        count\n"
		      "   -d                        don't fragment bit\n"
		      "   -n                        don't resolve hostname\n"
		      "   -r                        don't route\n"
		      "   -f                        print differential id\n"
		      "   -v                        print version\n" 
		      "   -h                        print this help\n" 
		      "   -l plugin                 load plugin (-ls list)\n"                      
                      "   -b                      * print link list\n", name);
	     break;
	 case USAGE_TOS:
	     fprintf (stderr, 
			"Actual Tos implementation:\n"
			"   IPTOS_LOWDELAY            0x10\n" 
			"   IPTOS_THROUGHPUT          0x08\n" 
			"   IPTOS_RELIABILITY         0x04\n" 
			"   IPTOS_MINCOST             0x02\n");
	     break;
	 case USAGE_ICMP_TYPE:
	     {
		 int           i;

		 fprintf (stderr, "Summary of message types and current implementation:\n");
		 fprintf (stderr, "\nlegenda: [_]= not implemented   [i]= incomplete\n"
				    "         [a] = alpha            [b]= beta          [*]= done\n\n"
				    "         [T][R] T= trasmitter, R= receiver\n\n");
		 for (i = 0; i < 64; i++)
		     {
			 if (icmp_type_str[i] != NULL)
			     printf ("         [%c][%c] -t %2d  %s\n", 
					maturity_level[i][0],
					maturity_level[i][1],	
					i, icmp_type_str[i]);

		     }
	     }

	     break;
	 case USAGE_ICMP_CODE:
	     {
		 int           i;
		 int           j;

		 fprintf (stderr, "Summary of Message Codes");

		 if (options.icmp_type)
		     {
			 i = icmp_type;

			 printf(" for icmp=%d(%s):\n\n",i,icmp_type_str[i]);	
		
			 for (j = 0; j < 64; j++)
			     {
				 if (icmp_code_str[(i<<8)  + j] != NULL)
				     printf ("   -k %2d  %s\n", j, icmp_code_str[(i<<8) + j]);

			     }
		     }
		 else
		     {

		     printf(":\n\n");
			
		     for (i = 0; i < 64; i++)
			 for (j = 0; j < 64; j++)
			     {
				 if (icmp_code_str[ INDEX(i,j) ] != NULL)
				     printf ("   -t %2d -k %2d  %s\n", i, j, icmp_code_str[INDEX(i,j)]);

			     }
		      }
	     }

	     break;

	 case USAGE_BANDWIDTH:
	     {
		 int           i = 1;

		 fprintf (stderr, "List of known link:\n");

		 while (link_vector[i].type != NULL)
		     fprintf (stderr, "   %s\n", link_vector[i++].type);

	     }
	     break;

	}

    exit (-1);

}
