/* ICMP_TIME_EXCEEDED */
 
#include "dissect.h" 
#include "aping.h"

#include "typedef.h"
#include "prototype.h"
#include "global.h"

static int _dissect_type = ICMP_TIMXCEED;
#include "maturity.h"

void
load_time_exceeded(packet *p, char **argv)
{
        /* maturity level */
        SET_LOADER_LEVEL('_');
}

void
dissect_time_exceeded(packet *p)
{
        /* maturity level */
        SET_DISSECT_LEVEL('_');
}
