#include "resolver.h"

int resolve_host (char *host, struct sockaddr_in *sa)
{
        struct hostent *ent ;
        memset(sa,0,sizeof(struct sockaddr));
        sa->sin_family = AF_INET;
        if (inet_addr(host) == -1)
        {
                ent = gethostbyname(host);
                if (ent != (void*)0)
                {
                        sa->sin_family = ent->h_addrtype;
			memcpy((caddr_t)&sa->sin_addr,ent->h_addr,ent->h_length);
			return 1;
		}
		else
			return  0;
	}
	return 1;
}
