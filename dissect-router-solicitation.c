/* ICMP ROUTER SOLICITATION */
 
#include "dissect.h" 
#include "aping.h"

#include "typedef.h"
#include "prototype.h"
#include "global.h"

static int _dissect_type = ICMP_ROUTERSOLICIT;
#include "maturity.h"

void
load_router_solicitation (packet *p, char **argv)
{
        /* maturity level */
        SET_LOADER_LEVEL('_');
}

void
dissect_router_solicitation (packet *p)
{
        /* maturity level */
        SET_DISSECT_LEVEL('_');

}
