/* ICMP_DEST_UNREACH */
 
#include "dissect.h" 
#include "aping.h"

#include "typedef.h"
#include "prototype.h"
#include "global.h"

void
load_destination_unreachable(packet *p, char **argv)
{
}

void
dissect_destination_unreachable(packet *p)
{

 bandwidth_predictor(p);

}
