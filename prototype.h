#if defined(__STDC__) || defined(__cplusplus)
# define _P(s) s
#else
# define _P(s) ()
#endif

/* aping.c */
__inline void print_icon _P((int t));
void ctrlc _P((int i));
void discard_plugin _P((void));
void defaults _P((void));
void report _P((void));
int main _P((int argc, char **argv));

/* argscheck.c */
__inline int checknum _P((char *smt));
__inline int checkip _P((char *smt));
__inline int checkport _P((char *smt));
__inline int checkportrange _P((char *smt));
__inline int checkiprange _P((char *smt));
int checkargs _P((char **argv, char minargsnum, ...));

/* chksum.c */
u_short chksum _P((u_short *addr, int len));

/* datalink.c */
int sizeof_datalink _P((pcap_t *pd));

/* dissect-destination.c */
void load_destination_unreachable _P((packet *p, char **argv));
void dissect_destination_unreachable _P((packet *p));

/* dissect-echo-reply.c */
void load_echo_reply _P((packet *p, char **argv));
void dissect_echo_reply _P((packet *p));

/* dissect-echo.c */
void load_echo _P((packet *p, char **argv));
void dissect_echo _P((packet *p));

/* dissect-information-reply.c */
void load_information_reply _P((packet *p, char **argv));
void dissect_information_reply _P((packet *p));

/* dissect-information.c */
void load_information_request _P((packet *p, char **argv));
void dissect_information_request _P((packet *p));

/* dissect-mask-reply.c */
void load_address_mask_reply _P((packet *p, char **argv));
void dissect_address_mask_reply _P((packet *p));

/* dissect-mask.c */
void load_address_mask_request _P((packet *p, char **argv));
void dissect_address_mask_request _P((packet *p));

/* dissect-parameter-problem.c */
void load_parameter_problem _P((packet *p, char **argv));
void dissect_parameter_problem _P((packet *p));

/* dissect-redirect.c */
void load_redirect _P((packet *p, char **argv));
void dissect_redirect _P((packet *p));

/* dissect-router-advertisement.c */
void load_router_advertisement _P((packet *p, char **argv));
void dissect_router_advertisement _P((packet *p));

/* dissect-router-solicitation.c */
void load_router_solicitation _P((packet *p, char **argv));
void dissect_router_solicitation _P((packet *p));

/* dissect-security.c */
void load_security _P((packet *p, char **argv));
void dissect_security _P((packet *p));

/* dissect-source-quench.c */
void load_source_quench _P((packet *p, char **argv));
void dissect_source_quench _P((packet *p));

/* dissect-time-exceeded.c */
void load_time_exceeded _P((packet *p, char **argv));
void dissect_time_exceeded _P((packet *p));

/* dissect-timestamp-reply.c */
void load_timestamp_reply _P((packet *p, char **argv));
void dissect_timestamp_reply _P((packet *p));

/* dissect-timestamp.c */
long iptime _P((void));
void load_timestamp _P((packet *p, char **argv));
void dissect_timestamp _P((packet *p));

/* fnv.c */
__inline unsigned long hash _P((const char *p, int s));

/* hardware.c */
char *getmacfrom6b _P((char *hmac, char *mac));
char *getmacfromdatalink _P((char *dl, int type));

/* iface.c */
int get_first_hop _P((long target, long *source, char *ifname));
long gethostbyif _P((char *ifname));

/* keystroke.c */
void keystroke _P((void));

/* pcap.c */
int pcap_setnonblock _P((pcap_t *p, int nonblock, char *errbuf));
int pcap_getnonblock _P((pcap_t *p, char *errbuf));

/* plugin.c */
void plugin_init _P((char *name));
void plugin_ls _P((void));

/* pthread.c */
int pthread_sigset_block _P((int n, ...));
int pthread_sigset_unblock _P((int n, ...));

/* receiver.c */
void lineup_layers _P((char *buff, packet *pkt));
unsigned short ntohss _P((unsigned short s));
__inline void agent_timestamp _P((packet *p));
void reset_ipid _P((void));
void reset_stat _P((void));
__inline int agent_ipid _P((packet *p));
__inline void agent_dyn_id _P((packet *p));
void print_RR _P((char *opt));
void process_pack _P((packet *p));
void receiver _P((void));

/* resolver.c */
long gethostbyname_lru _P((const char *host));
char *gethostbyaddr_lru _P((unsigned long addr));
char *multi_inet_ntoa _P((long address));

/* sender.c */
int sizeof_icmp _P((int t));
void load_layers _P((char *buff, packet *pkt));
void load_ip _P((struct ip *ip));
void sender _P((char **argv));

/* socket.c */
int atomic_sendto _P((int fd, char *_s, int n, struct sockaddr *saddr));

/* statistic.c */
char *link_type _P((long kb));
long E _P((long *addr, long data, long n));
unsigned char TTL_PREDICTOR _P((unsigned char x));
long twostep_predictor _P((long raw, long pen_time));
double onestep_FIR _P((double n));
long lp_FIR _P((long kbps, long pt));
long magic_round _P((long a, long b));
void bandwidth_predictor _P((packet *p));

/* strlcat.c */
size_t strlcat _P((char *dst, const char *src, size_t siz));

/* strlcpy.c */
size_t strlcpy _P((char *dst, const char *src, size_t siz));

/* termios.c */
void termios_set _P((void));
void termios_reset _P((void));

/* usage.c */
void usage _P((char *name, int type));

#undef _P
