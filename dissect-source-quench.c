/* ICMP_SOURCE_QUENCH */
 
#include "dissect.h" 
#include "aping.h"

#include "typedef.h"
#include "prototype.h"
#include "global.h"

static int _dissect_type = ICMP_SOURCEQUENCH;
#include "maturity.h"

void
load_source_quench(packet *p, char **argv)
{
        /* maturity level */
        SET_LOADER_LEVEL('_');
}

void
dissect_source_quench(packet *p)
{
        /* maturity level */
        SET_DISSECT_LEVEL('_');
}
