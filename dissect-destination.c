/* ICMP_DEST_UNREACH */
 
#include "dissect.h" 
#include "aping.h"

#include "typedef.h"
#include "prototype.h"
#include "global.h"

/*

Destination Unreachable Message

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Type      |     Code      |          Checksum             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                             unused                            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |      Internet Header + 64 bits of Original Data Datagram      |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

*/

void
load_destination_unreachable(packet *p, char **argv)
{
	/* redirect_type , tos,icmp_src_ip, icmp_dest_ip */
	checkargs(argv,4,ARG_NUM,ARG_NUM,ARG_IP,ARG_IP,ARG_IP);

	ICMP_type(p)= ICMP_REDIRECT;
	ICMP_code(p)= ((int)strtol(argv[0], (char **)NULL, 10))%4;

	ICMP_IP_ver(p)  = 4; 
	ICMP_IP_hl(p)   = 5; /*no options*/
	ICMP_IP_tos(p)  = ((int)strtol(argv[1], (char **)NULL, 10));
	ICMP_IP_len(p)  = htons(sizeof(struct ip)+64+20);
	ICMP_IP_id(p)   = htons(1+(int) (65535.0*rand()/(RAND_MAX+1.0))); 
	ICMP_IP_off(p)  = 0;
	ICMP_IP_ttl(p)  = 255;
	ICMP_IP_p(p)    = IPPROTO_TCP;
	ICMP_IP_sum(p)  = htons(1+(int) (65535.0*rand()/(RAND_MAX+1.0))); 
	ICMP_IP_src(p)  = gethostbyname_lru(argv[2]);
	ICMP_IP_dst(p)  = gethostbyname_lru(argv[3]);


	/*since bsd kernel don't give shit about 64 bits we don't care and fill with shit */
	ICMP_TCP_sport(p) = htons(1+(int) (65535.0*rand()/(RAND_MAX+1.0))); 
	ICMP_TCP_dport(p) = htons((short)(ICMP_TCP_sport(p)+0xfaded));


	ICMP_sum(p)= 0;
	ICMP_sum(p)= chksum((u_short *)p->icmp, sizeof_icmp(ICMP_ECHO));

}

void
dissect_destination_unreachable(packet *p)
{

 bandwidth_predictor(p);

}
