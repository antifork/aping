char pi_name[]   = PACKAGE_NAME;
char pi_string[] = PACKAGE_STRING;
char pi_traname[]= PACKAGE_TARNAME;
char pi_version[]= PACKAGE_VERSION;
char pi_bugrep[] = PACKAGE_BUGREPORT;
char pi_info[]   = PACKAGE_INFO;

#define FATAL(f,arg...) {                                               \
                        fprintf(stderr,"%s:%d: ",__FILE__,__LINE__);    \
                        fprintf(stderr,f,## arg);                       \
                        fputs (". exit-forced\n",stderr);               \
                        exit(1);                                        \
                        }
