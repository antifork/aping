#include "wmdata.h"
#include "macro.h"
/*
	OCCHIO MANCANO LE SINCRONIZZAZIONI!!
	E NON FUNZIONA ANCORA UN CAZZO!!
	NON INCLUDERE NULLA ANCORA AW... 
	HO DOVUTO RISCRIVERE TUTTO E SEGARE IL GRABAGE COLLECTOR..
	SE HAI DUE MINUTI APRI IL MAN DI shmctl E FATTI UNA RISATA...
*/

/*
	torna una cll per il motivo sopra...
	


        0xfaded
        ________
       |        |---------------------tail---.
       | WMROOT |    0xfaded+1               | 0xfaded+2
       |________|     ________           ____V___        
           |         |        |         |        |       
           L--head-->| WMDATA |-------->| WMDATA |---- - - - - .
      .------------->|________|         |________|             | 
      |                 |     ______       |     ______        |  
      |                 |    |      |      |    |      |       |  
      |                 L----| DATA |      L----| DATA |       |  
      |               0xbea+1|______|    0xbea+2|______|       |  
      |________________________________________________________|
*/                               

char rehashing;

WMROOT* get_root()
{
	int s_mem_id;
	char* pmem;	
	if ((s_mem_id = shmget( 0xfaded, sizeof(WMROOT), SHM_R|SHM_W)) < 0)		
		return (WMROOT*)0;
	if ((pmem = shmat(s_mem_id, NULL, 0)) == (char *) -1)
		return (WMROOT*)0;
	return(WMROOT*)pmem;
}

WMDATA* get_data_from_key(key_t key)
{
	int s_mem_id;
	char* pmem;	
	if ((s_mem_id = shmget( key, sizeof(WMDATA), SHM_R|SHM_W)) < 0)		
		return (WMDATA*)0;
	if ((pmem = shmat(s_mem_id, NULL, 0)) == (char *) -1)
		return (WMDATA*)0;
	return(WMDATA*)pmem;
}

void* get_data_value_from_key(WMDATA* wmdata)
{
	int s_mem_id;
	char* pmem;	
	if ((s_mem_id = shmget( wmdata->dataname, wmdata->len, SHM_R)) < 0)
		return (void*)0;

	if ((pmem = shmat(s_mem_id, NULL, 0)) == (char *) -1)
		return (void*)0;
	return (void*)pmem;
}




WMROOT* watermark_init(int plugins,int isserver)
{
	int s_mem_id;
	key_t name;
	char* pmem;
	int sem_id;
	union semun
	{
		int val;
		struct semid_ds *buf;
		ushort * array;
	} argument;
	argument.val = 0;
	
	name = 0xfaded;
	rehashing= 0;
	if ((sem_id= semget(0xdeadbeef, 1, IPC_CREAT|SHM_R|SHM_W)) < 0)
	{
		FATAL("semget");
	}
	if( semctl(sem_id, 0, SETVAL, argument) < 0)
   	{
		FATAL("semctl");
	}
	
	if ((s_mem_id = shmget( name, sizeof(WMROOT), IPC_CREAT|SHM_R|SHM_W)) < 0)
	{
		FATAL("shmget");
	}

	if ((pmem = shmat(s_mem_id, NULL, 0)) == (char *) -1)
	{
		FATAL("shmat");
	}

	if(isserver)
	{
		((WMROOT*)pmem)->wm_name_   =0xfaded;
		((WMROOT*)pmem)->wm_id_     =s_mem_id;
		((WMROOT*)pmem)->wm_head_   =0;
		((WMROOT*)pmem)->wm_tail_   =0;
		((WMROOT*)pmem)->wm_len_    =0;	
		((WMROOT*)pmem)->wm_plugins_=plugins;	
	}
	return (WMROOT*)pmem;
}

void
watermark_push(void* data, unsigned int len)
{
	int   s_mem_id , s_mem_id2;
	key_t name     , name2 ;
	char* pmem     , *pdata;
	WMROOT* wmroot;
	WMDATA* prev;
	
	
	wmroot = get_root();
	if(wmroot->wm_tail_==0xfaded+256)
	{
		wmroot->wm_tail_=0xfaded+1;
		rehashing=1;
		prev=get_data_from_key(0xfaded+255);
		prev->wmd_next_=0xfaded+1;
	}else{
		if(!wmroot->wm_head_)	
		{
			wmroot->wm_head_=0xfaded+1;
			wmroot->wm_tail_=0xfaded+1;
		}else
		{
			wmroot->wm_tail_+=1;
			name=wmroot->wm_tail_;
			prev=get_data_from_key(wmroot->wm_tail_-1);
			prev->wmd_next_=wmroot->wm_tail_;
		}
	}

	/* creating WMDATA*/	
	if ((s_mem_id = shmget( wmroot->wm_tail_, sizeof(WMDATA), IPC_CREAT|SHM_R|SHM_W)) < 0)
	{
		FATAL("shmget");
	}

	if ((pmem = shmat(s_mem_id, NULL, 0)) == (char *) -1)
	{
		FATAL("shmat");
	}
	name2=0xbea+0xfaded-wmroot->wm_tail_;
	/* creating data */
	if ((s_mem_id2 = shmget(name2, len, IPC_CREAT|SHM_R|SHM_W)) < 0)
	{
		FATAL("shmget");
	}

	if ((pdata = shmat(s_mem_id2, NULL, 0)) == (char *) -1)
	{
		FATAL("shmat");
	}

	memcpy(pdata,data,len);
	
	wmroot->wm_len_++;
	((WMDATA*)pmem)->wmd_id_   =s_mem_id;
	((WMDATA*)pmem)->wmd_name_ =name;
	((WMDATA*)pmem)->wmd_next_ =0;
	((WMDATA*)pmem)->wmd_references_=wmroot->wm_plugins_;
	((WMDATA*)pmem)->data_key=name2;
	((WMDATA*)pmem)->len=len;
	((WMDATA*)pmem)->dataname=name2;	
	((WMDATA*)pmem)->data=pdata;	
}

WMDATA* watermark_pop(key_t key)
{
	int   s_mem_id;
	WMROOT* wmroot;
	WMDATA* wdata;
	char* pmem;
	if (key==0)
	{
		wmroot     =get_root();
		wdata      =get_data_from_key(wmroot->wm_head_);	
	}
	wdata      =get_data_from_key(key);
	wdata->data=get_data_value_from_key(wdata);
	wdata->wmd_references_--;
	/* TBD: WE NEED TO COPY THE DATA SINCE WE WANT IT TO BE rescheduled in the arena */
	return wdata;
}


void watermark_garbage_collector()
{
	/*
	il garbage collector Š inutile a questo punto... 
	*/
}

void watermark_delete(int s_mem_id)
{
/*tbd*/
}
