#define FATAL(f,arg...) {                                               \
                        fprintf(stderr,"%s:%d: ",__FILE__,__LINE__);    \
                        fprintf(stderr,f,## arg);                       \
                        fputs (". exit-forced\n",stderr);               \
                        exit(1);                                        \
                        }

