#include "../../filter.h"

#define FATAL(f,arg...) {                                               \
fprintf(stderr,"%s:%d %s(): ",__FILE__,__LINE__,__FUNCTION__);          \
fprintf(stderr,f,## arg);                                               \
fprintf(stderr,"\n");                                                   \
exit(1);                                                                \
}


#define PI_SIGNAL_INIT(s)	{				\
sigemptyset ((s));						\
sigaddset   ((s), SIGTSTP);					\
sigaddset   ((s), SIGINT);					\
sigaddset   ((s), SIGQUIT);					\
sigprocmask (SIG_BLOCK, (s), NULL);	}

#define PI_SIGNAL_EXIT(n)	{				\
signal(SIGUSR1,CAT(n,_exit));	}
