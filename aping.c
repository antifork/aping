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

#define   GLB_OWNER
#include "global.h"
#include "maturity.h"

static
const char cvsid[] = "$Id$";

extern char pcap_version[];
extern int allow_reverse;

static
char icon_set[] = "->DZL>";

/*
 * print icon
 */
__inline void
print_icon(int t)
{
	printf("\r%c", icon_set[!t * (n_sent & 1) + t]);
	fflush(stdout);
}


/*
 * ctr+c handler
 */
void
ctrlc(i)
	int i;
{
	if (options.sniff) {
		termios_reset();
		PUTS("done.\n");
		exit(0);

	}
	pthread_cancel(pd_snd);
}


/*
 * discard plugin
 */
void
discard_plugin()
{
	int k = 0;

	for (; k < MIN(pd_pindex, MAX_CHILD); k++) {
		kill(pd_plugin[k], SIGUSR1);
	}
}


/*
 * load defaults
 */
void
defaults()
{
	struct rlimit core;
	int i;

	/* rlimit */

	core.rlim_cur = 0;
	core.rlim_max = 0;

	/* setrlimit */
#ifndef EADBUG
	setrlimit(RLIMIT_CORE, &core);
#endif
	/* maturity interface */
	SETUP_MATURITY();
	LOAD_MATURITY();

	/* refresh salt */
	srand(time(NULL));

	/* default pattern */
	pattern = (char *) malloc(49);

	for (i = 0; i < 48; i++)
		pattern[i] = i + 8;	/* Berkeley style */

	pattern[48] = 0;

	/* set myid */
	myid = getpid() & 0xffff;

	/* ns-cache */
	allow_reverse = 1;
}


/*
 * print report
 */
void
report()
{
	int sleep_time;
	int loss;

	sleep_time = (n_tome ? 100 + rtt_mean + 2 * ISQRT(rtt_sqre - rtt_mean * rtt_mean) : 2000);

	PUTS("--- sleeping %d ms (RTT+2 sigma) ---\n", sleep_time);
	usleep(sleep_time * 1000);
	PUTS("\n--- %s aping statistics ---\n", safe_inet_ntoa(ip_dst));

	loss = 100 - PER_CENT(n_tome, n_sent);

	if (loss < 0)
		PUTS("%ld packets transmitted, %ld packets received (forked responses found)\n",
		     n_sent, n_tome);
	else
		PUTS("%ld packets transmitted, %ld packets received, %d %% packets loss\n",
		     n_sent, n_tome, loss);
	if (n_tome)
		PUTS("round-trip min/mean/dstd/max = %ld/%ld/%ld/%ld ms\n", rtt_min, rtt_mean,
		     ISQRT(rtt_sqre - rtt_mean * rtt_mean), rtt_max);
}



/*
 * aping
 */
int
main(argc, argv)
	int argc;
	char **argv;
{
	int es;
	int major;
	int minor;

	/* set values by default */

	defaults();

	atexit(discard_plugin);

	/* security */
	if (getuid() != geteuid())
		fatal("aping doesn't run with +s bit set");

	if (getuid() != 0)
		fatal("aping must run as root");

	if (argc < 2)
		fatal("no arguments given");

	if (argv[1][0] == '-' && argv[1][1] == '-')
		fatal("%s doesn't support --long-options.\ntype %s -h instead", argv[0], argv[0]);

	/* parse options */
	while ((es = getopt(argc, argv, "DS:O:T:MRI:t:k:i:c:p:e:Pz:l:dnrvhbys")) != EOF)
		switch (es) {
		case 'S':
			SET(ip_src);
			host_src = strdup(optarg);
			break;
		case 'O':
			if (!strcmp("help", optarg))
				usage(optarg, USAGE_TOS);

			SET(ip_tos);
			ip_tos = ATOA(optarg);
			break;
		case 'T':
			SET(ip_ttl);
			ip_ttl = ATOA(optarg);
			break;
		case 'M':
			SET(ip_ramp);
			break;
		case 'R':
			SET(rroute);
			break;
		case 'I':
			if (argv[optind] == NULL && *optarg == 'd')
				fatal("option requires an argument -Id int");
			switch (*optarg) {
			case 'd':
				SET(ip_id);
				ip_id = ATOA(argv[optind++]);
				break;
			case 'i':
				SET(ip_id_incr);
				break;
			case 'r':
				SET(ip_id_rand);
				break;
			default:
				fatal("invalid option -I%c", *optarg);
				break;
			}
			break;
		case 'b':
			SET(bcast);
			break;
		case 'y':
			usage(optarg, USAGE_BANDWIDTH);
			break;
		case 'l':
			if (!strcmp("s", optarg))
				plugin_ls();

			plugin_init(optarg);
			break;
		case 't':
			if (!strcmp("help", optarg))
				usage(optarg, USAGE_ICMP_TYPE);

			SET(icmp_type);
			icmp_type = ATOA(optarg);
			break;
		case 'k':
			if (!strcmp("help", optarg))
				usage(optarg, USAGE_ICMP_CODE);

			SET(icmp_code);
			icmp_code = ATOA(optarg);
			break;
		case 'i':
			SET(wait);
			tau = ATOA(optarg);
			break;
		case 'c':
			SET(count);
			count = ATOA(optarg);
			break;
		case 'p':
			SET(pattern);
			free(pattern);
			pattern = strdup(optarg);
			break;
		case 'e':
			SET(ifname);
			strncpy(ifname, optarg, 16);
			break;
		case 'P':
			SET(promisc);
			break;
		case 's':
			SET(sniff);
			break;
		case 'z':
			SET(size);
			mysize = ATOA(optarg);
			break;
		case 'd':
			SET(so_debug);
			break;
		case 'n':
			SET(numeric);
			allow_reverse = 0;
			break;
		case 'r':
			SET(droute);
			break;
		case 'D':
			SET(dfrag);
			break;
		case 'v':
			sscanf(pcap_version, "%d.%d", &major, &minor);
			PUTS("aping %s (pcap version %d.%d)\n", VERSION, major, minor);
			exit(0);
			break;
		case '?':
			exit(-1);
			break;
		case 'h':
			usage(argv[0], USAGE_CLASSIC);
			break;
		}

	argc -= optind;
	argv += optind;

	ATOMIC_TEST(sniff, ip_src);
	ATOMIC_TEST(sniff, ip_dst);
	ATOMIC_TEST(ip_id_rand, ip_id_incr);
	DEPEND_TEST(sniff, ifname);
	DEPEND_TEST(ip_src, ifname);

	if (options.ip_src) {
		ip_src = gethostbyname_cache(host_src);
		if (ip_src == -1)
			fatal("bad source host");
	}
	/* destination.. */
	if (*argv != NULL) {
		host_dst = strdup(*argv);
		ip_dst = gethostbyname_cache(*argv);
		if (ip_dst == -1)
			fatal("bad destination host.");

	} else if (!options.sniff)
		fatal("no destination given");

	if (!options.ip_src) {
		if (!options.ifname)
			get_first_hop(ip_dst, &ip_src, ifname);
		else
			ip_src = gethostbyif(ifname);

	}
	/* get localnet/netmask */
	if (pcap_lookupnet(ifname, &localnet, &netmask, bufferr) == -1)
		fatal(bufferr);

	if (pthread_create(&pd_rcv, NULL, (void *) receiver, NULL) != 0)
		fatal("pthread_create(): %s", strerror(errno));

	if (pthread_create(&pd_key, NULL, (void *) keystroke, NULL) != 0)
		fatal("pthread_create(): %s", strerror(errno));

	/* Catch signal */
	ssignal(SIGINT, SIG_IGN);
	ssignal(SIGTSTP, SIG_IGN);
	ssignal(SIGQUIT, SIG_IGN);
	ssignal(SIGALRM, SIG_IGN);

	/* set termios properties */
	termios_set();

	if (!options.sniff) {
		if (pthread_create(&pd_snd, NULL, (void *) sender, argv) != 0)
			fatal("pthread_create(): %s", strerror(errno));

		/* waiting for pd_snd exit/cancel */
		pthread_join(pd_snd, NULL);
	} else
		pthread_join(pd_rcv, NULL);

	/* reset termios properties */
	termios_reset();

	if (n_sent == 0) {
		PUTS("done.\n");
		exit(0);
	}
	signal(SIGTSTP, SIG_DFL);
	signal(SIGINT, SIG_DFL);

#if 0
	pthread_cancel(pd_rcv);
#endif
	report();
	exit(0);
}
