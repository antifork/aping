#include "../../macro.h"
#include "../../filter.h"


#define PI_SIGNAL_INIT(s)	{				\
sigemptyset ((s));						\
sigaddset   ((s), SIGTSTP);					\
sigaddset   ((s), SIGINT);					\
sigaddset   ((s), SIGQUIT);					\
sigprocmask (SIG_BLOCK, (s), NULL);	}

#define PI_SIGNAL_EXIT(n)	{				\
signal(SIGUSR1,CAT(n,_exit));	}
