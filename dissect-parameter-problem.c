/* ICMP_PARAMETERPROB */
 
#include "dissect.h" 
#include "aping.h"

#include "typedef.h"
#include "prototype.h"
#include "global.h"

static int _dissect_type = ICMP_PARAMPROB;
#include "maturity.h"


void
load_parameter_problem (packet *p, char **argv)
{
        /* maturity level */
        SET_LOADER_LEVEL('_');

}

void
dissect_parameter_problem (packet *p)
{
        /* maturity level */
        SET_DISSECT_LEVEL('_');

}
