#include "config.h"

#ifndef STUB_PCAP_H
#define STUB_PCAP_H
 
#ifdef HAVE_PCAP_H
#include <pcap.h>
#elif HAVE_PCAP_PCAP_H
#include <pcap/pcap.h>
#else
#error "pcap.h header not installed"
#endif

#endif /* STUB_PCAP_H */
