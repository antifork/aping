#include "wmdata.h"

static unsigned const poly= 0x04c11db7U;
static unsigned int eth_crc(int length, char *data)
{
	int bit,crc = -1;
	char current_octet;
	while (--length >= 0)
	{
		current_octet = *data++;
		for (bit = 0; bit < 8; bit++, current_octet >>= 1)
		crc=(crc<<1)^((crc<0)^(current_octet &1)?poly:0);
	}
 	return crc;
}

SHARED_ARENA* watermark_init(int isserver)
{
	int s_mem_id;
	int init_counter;
	char* pmem;

	if(isserver)
	{
		if ((s_mem_id = shmget(ARENA_KEY, sizeof(SHARED_ARENA), IPC_CREAT|IPC_EXCL|SHM_R|SHM_W|(SHM_R>>6))) < 0)
		{
			FATAL("Cannot get Shm. Server is running?\r\n");
		}
	}else{
		if ((s_mem_id = shmget( ARENA_KEY, sizeof(SHARED_ARENA), 0 )) < 0)
		{
			FATAL("Cannot get Shm. Server is running?");
		}
	}

	if ((pmem = shmat(s_mem_id, NULL, 0)) == (char *) -1)
	{
		FATAL("shmat1");
	}

	if(isserver)
	{
		memset(pmem,0,sizeof(SHARED_ARENA));
		for(init_counter=0;init_counter!=ARENA_SIZE;init_counter++)
			((SHARED_ARENA*)pmem)->buckets[init_counter].id==1;
		((SHARED_ARENA*)pmem)->buckets[1].id==2;				
		((SHARED_ARENA*)pmem)->size=ARENA_SIZE;
	}
	return (SHARED_ARENA*)pmem;
}

void
watermark_push(SHARED_ARENA* sha, void* data, unsigned int len, int oldid)
{
	
	int tlen;
	unsigned int crcval;
	BUCKET bucket_tmp;
	
	/*flagging*/
	oldid++;
	sha->buckets[(oldid)%ARENA_SIZE].id=oldid;

	tlen=(len>ARENA_SIZE)?ARENA_SIZE:len;
	bucket_tmp.crc  =eth_crc(tlen,data);
	bucket_tmp.id   =oldid;
	bucket_tmp.wrote=oldid;
	bucket_tmp.len  =tlen;
	memcpy(bucket_tmp.buffer,data,tlen);
	
	
	/*swapping*/	
	memcpy(&sha->buckets[(oldid)%ARENA_SIZE],&bucket_tmp,sizeof(BUCKET));
}

/*
	will return the BUCKET in return_bucket and return id read 
*/
unsigned int 
watermark_pop(SHARED_ARENA* sha,BUCKET* return_bucket ,int msg_id)
{
	int tlen;
	BUCKET bucket_tmp;
	
	for(;;)
	{
		/* blindreading */
		memcpy(	return_bucket, &sha->buckets[msg_id%ARENA_SIZE], sizeof(BUCKET));
	
		/*testing*/
		if (return_bucket->id==return_bucket->wrote && return_bucket->id >=msg_id)
		{
			if (return_bucket->crc==eth_crc(return_bucket->len,return_bucket->buffer))
			{
				return return_bucket->id+1;
			}
		}
		usleep(100000);
	}
}


void watermark_delete()
{
	int s_mem_id;
	if ((s_mem_id = shmget( ARENA_KEY, sizeof(SHARED_ARENA), SHM_R |SHM_W)) < 0)		
	{
		FATAL("Cannot get block\r\n");
	}
	shmctl(s_mem_id, IPC_RMID, 0);
	printf("Shared memory block removed\r\n");
	return;
}





#ifdef TRY_SHM_SERVER
/*
	gcc -o server -DTRY_SHM_SERVER wmdata.c && gcc -o client -DTRY_SHM_CLIENT wmdata.c
	./server 
	./client
	
	to test.
*/

int main()
{
	SHARED_ARENA* sha;
	int msg_id=SERVER_INIT_MSG_ID;
	char message[15];
	printf("init\r\n");
	sha=watermark_init(APING_SERVER);
	for(;msg_id!=1000;msg_id++)
	{
		sprintf(message,"mex=%d",msg_id);
		printf("pushing %s\r\n",message);
		watermark_push(sha,message,strlen(message)+1,msg_id);
		sleep(1);
	}
	watermark_delete();
}
#endif

#ifdef TRY_SHM_CLIENT
int main()
{
	SHARED_ARENA* sha;
	BUCKET bucket;
	unsigned int msg_id=CLIENT_INIT_MSG_ID;
	printf("init\r\n");
	sha=watermark_init(APING_PLUGIN);
	printf("trying to pop\r\n");
	for(;msg_id<=1000;)
	{
		printf("trying to pop msg=%d\r\n",msg_id);
		fflush(stdin);
		msg_id=watermark_pop(sha,&bucket,msg_id);
		printf("popped: %s\r\n",bucket.buffer);
	}
}
#endif
#ifdef CLEAN_SHARED
int main()
{
	watermark_delete();
}
#endif
