#include "argscheck.h"

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
	char octet[4];
	int dots=0;
	int pivot;
	char *str;
	int stl=strlen(smt);
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
	int paramnum;
	char format;
	char**args;
	args=argv;
	while(*args)
	{
		paramnum++;
		args++;
	}
	if(paramnum<minargsnum) return -1;
 	va_start(ap, minargsnum);

	for(paramnum=0;paramnum!=minargsnum;paramnum++)
	{
		format=va_arg(ap,char);
		switch(format)
		{
			case ARG_NUM:
				if (!checknum(argv[paramnum]))
				{
					 fprintf(stderr,"argument %s is not a valid number\n",argv[paramnum]);
					 return -1;
				}
			break;
			
			case ARG_IP:
				if (!checkip(argv[paramnum]))
				{
					 fprintf(stderr,"argument %s is not a valid ip\n",argv[paramnum]);
					 return -1;
				}
			break;

			case ARG_IP_RANGE:
				if (!checkiprange(argv[paramnum]))
				{
					 fprintf(stderr,"argument %s is not a valid ip or ip_range\n",argv[paramnum]);
					 return -1;
				}
			break;

			case ARG_PORT:
				if (!checkport(argv[paramnum]))
				{
					 fprintf(stderr,"argument %s is not a valid port number\n",argv[paramnum]);
					 return -1;
				}
			break;

			case ARG_PORT_RANGE:
				if (!checkportrange(argv[paramnum]))
				{
					 fprintf(stderr,"argument %s is not a valid port range\n",argv[paramnum]);
					 return -1;
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
