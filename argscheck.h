#ifndef ARGSCHECK_H_
#define ARGSCHECK_H_
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>


#define TEST_ARG
#define ARG_ANY  	  0x01
#define ARG_INT  	  0x02
#define ARG_NUM  	  0x04
#define ARG_IP 		  0x08
#define ARG_IP_RANGE      0x10
#define ARG_PORT          0x20
#define ARG_PORT_RANGE    0x40

int checknum (char*);
int checkip  (char*);
int checkport(char*);
int checkportrange(char*);
int checkiprange  (char*);
int checkargs(char**,char,...);

#endif
