/* sm_ds.h share memory data structure */

#ifndef __SHM_DS_H__
#define __SHM_DS_H__

#ifndef SIZE_SHARE_MEM
#define SIZE_SHARE_MEM (10)
#endif

#define NAME_SHARE_MEM "MyShareMem"

typedef struct shm_data_t
{
	volatile int s32write_guard;
	volatile int s32read_guard;
	volatile int s32delete_guard;
	volatile int as32data[SIZE_SHARE_MEM];
} shm_data;


char* getShareMemName(void); 
void rnd_populate(int* ps32ptr, int s32len);
void print_intlist(volatile int* ps32ptr, int s32len);
#endif
