#ifndef WMDATA_H_
#define WMDATA_H_

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>

#define WM_SERVER 0x01
#define WM_CLIENT 0x00

typedef struct watermarkroot_
{
	int   wm_id_;
	key_t wm_name_;
	key_t wm_head_;                                    
	key_t wm_tail_;                                    
	unsigned short wm_len_;
	unsigned short wm_plugins_;                        
	
}WMROOT;


typedef struct watermarkdata_
{
	int   wmd_id_;
	key_t wmd_name_;
	key_t wmd_next_;
	unsigned short wmd_references_;

	key_t data_key;
	key_t dataname;
	size_t len;
	void* data;
}WMDATA;

WMROOT* get_root();
WMDATA* get_data_from_key(key_t key);
void* get_data_value_from_key(WMDATA* wmdata);
WMROOT* watermark_init(int plugins,int isserver);
void watermark_push(void* data, unsigned int len);
WMDATA* watermark_pop(key_t key);


#endif
