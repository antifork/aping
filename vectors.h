#ifndef HAVE_VECTORS
#define HAVE_VECTORS

static 
void  (*icmp_loader_vector[256]) (packet *, char **) =
{                                                   
        load_echo_reply,
        NULL,
        NULL,
        load_destination_unreachable,
        load_source_quench,
        load_redirect,
        NULL,
        NULL,
        load_echo,
        load_router_advertisement,
        load_router_solicitation,
        load_time_exceeded,
        load_parameter_problem,
        load_timestamp,
        load_timestamp_reply,
        load_information_request,
        load_information_reply,
        load_address_mask_request,
        load_address_mask_reply
};

static
void  (*icmp_dissect_vector[256]) (packet *) =
{
        dissect_echo_reply,
        NULL,
        NULL,
        dissect_destination_unreachable,
        dissect_source_quench,
        dissect_redirect,
        NULL,
        NULL,
        dissect_echo,
        dissect_router_advertisement,
        dissect_router_solicitation,
        dissect_time_exceeded,
        dissect_parameter_problem,
        dissect_timestamp,
        dissect_timestamp_reply,
        dissect_information_request,
        dissect_information_reply,
        dissect_address_mask_request,
        dissect_address_mask_reply
};


#endif
