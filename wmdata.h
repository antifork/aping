#ifndef WMDATA_H
#define WMDATA_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(TRY_SHM_SERVER) || defined (TRY_SHM_CLIENT) || defined (CLEAN_SHARED)
	#include <unistd.h>
	#define FATAL(fatlmsg) { printf("%s",fatlmsg); exit(1); }
#else
 #include "macro.h"
#endif

#define APING_SERVER 0x01
#define APING_PLUGIN 0x00

#define CLIENT_INIT_MSG_ID 0
#define SERVER_INIT_MSG_ID -1

#define ARENA_SIZE  12
#define ARENA_KEY   0xfaded
typedef struct bucket_
{
	unsigned int id;
	unsigned int wrote;
	unsigned int crc;
	unsigned int len;
	char buffer[1024];
}BUCKET;


typedef struct shared_arena_ 
{
	unsigned int size;
	unsigned int resync[2];
	BUCKET buckets[ARENA_SIZE];
}SHARED_ARENA;

SHARED_ARENA* watermark_init(int);
void watermark_push(SHARED_ARENA* , void* , unsigned int , int );
unsigned int watermark_pop(SHARED_ARENA* ,BUCKET* ,int );
#endif
