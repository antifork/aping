#include "../macro.h"
#include "../filter.h"


#define PI_SIGNAL_INIT(s)	{				\
sigemptyset ((s));						\
sigaddset ((s), SIGTSTP);					\
sigaddset ((s), SIGINT);					\
sigaddset ((s), SIGQUIT);					\
pthread_sigmask (SIG_BLOCK, (s), NULL);	}

#define PI_PTHREAD_INIT(n)	{				\
pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL);           \
pthread_setcanceltype  (PTHREAD_CANCEL_ASYNCHRONOUS, NULL); }	

