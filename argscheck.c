#include "argscheck.h"
#include "macro.h"
#ifdef __GNUC__
 __inline
#endif
int checknum(char*smt)
{
        for(;*smt;smt++)if(!isdigit(*smt))return 0;
        return 1;
}


#define TEST_OCTET(pivot,smt) \
	if(*smt=='.')smt++; \
	for(pivot=0;pivot!=4;pivot++)octet[pivot]=0; \
        for(pivot=0;isdigit(*smt);pivot++,smt++) \
        { \
		if (pivot >3) return 0; \
        	octet[pivot]=*smt;	 \
        } \
	pivot=strtol(octet, (char **)NULL, 10); \
	if (pivot>255 || pivot < 0 ) return 0; \


#ifdef __GNUC__
 __inline
#endif
int checkip(char* smt)
{
	char *str,octet[4];
	int pivot,stl,dots=0;
	for(str=smt,stl=0;*str;stl++,str++);
	if(isalpha(*smt))return 1;
	if ( stl>15 ) return 0;
	if ( stl<7  ) return 0;
	str=smt;
	while(*str)
	{
		if (*str=='.')dots++;
		str++;
	}
	if(dots!=3) return 0;

       	TEST_OCTET(pivot,smt);
    	TEST_OCTET(pivot,smt);
    	TEST_OCTET(pivot,smt);
    	TEST_OCTET(pivot,smt);
	return 1;
}

#ifdef __GNUC__
 __inline
#endif
int
checkport(char* smt)
{
	int a=(int)strtol(smt, (char **)NULL, 10);
	if(a<1 || a>65535) return 0;
	return 1;
}

#ifdef __GNUC__
 __inline
#endif
int
checkportrange(char*smt)
{
	char* pvt;
	char pivot;
	int port;
	int port2;
	for(pvt=smt;*pvt && *pvt!='-';pvt++);
	if(!*pvt)return 0;
	pivot=*pvt;
	*pvt=0;
	port =strtol(smt, (char **)NULL, 10);
	port2=strtol(++pvt, (char **)NULL, 10);
	*(--pvt)=pivot;
	if (port>65535 || port <1 || port2>65535 || port2 <1) return 0;
	return 1;
}

#ifdef __GNUC__
 __inline
#endif
int
checkiprange(char*smt)
{
	char* pvt;
	char pivot;
	for(pvt=smt;*pvt && *pvt!='-';pvt++);
	if(!*pvt)return 0;
	pivot=*pvt;
	*pvt=0;
        checkip(smt);
        checkip(++pvt);
	*(--pvt)=pivot;
	return 1;
}

int
checkargs(char**argv,char minargsnum,...)
{
 	va_list ap;
	int paramnum=0;
	int format;
	char**args;
	args=argv;
	while(*args)
	{
		paramnum++;
		args++;
	}
	if(paramnum<minargsnum) FATAL("%d parameters requested",minargsnum);
 	va_start(ap, minargsnum);

	for(paramnum=0;paramnum!=minargsnum;paramnum++)
	{
		format=va_arg(ap,int);
		switch(format)
		{
			case ARG_NUM:
				if (!checknum(argv[paramnum]))
				{
					FATAL("argument %s is not a valid number",argv[paramnum]);
				}
			break;
			
			case ARG_IP:
				if (!checkip(argv[paramnum]))
				{
					 FATAL("argument %s is not a valid ip\n",argv[paramnum]);
				}
			break;

			case ARG_IP_RANGE:
				if (!checkiprange(argv[paramnum]))
				{
					 FATAL("argument %s is not a valid ip or ip_range\n",argv[paramnum]);
				}
			break;

			case ARG_PORT:
				if (!checkport(argv[paramnum]))
				{
					 FATAL("argument %s is not a valid port number\n",argv[paramnum]);
				}
			break;

			case ARG_PORT_RANGE:
				if (!checkportrange(argv[paramnum]))
				{
					 FATAL("argument %s is not a valid port range\n",argv[paramnum]);
				}
			break;			
			
			case ARG_ANY:
			default:
			break;
						
		}
	}
	va_end(ap);
	return 1;
}
