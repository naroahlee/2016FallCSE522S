#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "shm_ds.h"

int main(int argc, char* argv[])
{
	int s32shm_fd;
	int s32ret;
	void *ptr;
	shm_data *pstrshm;
	int* ps32local;

	/* Initalize Array */
	ps32local = (int*)malloc(SIZE_SHARE_MEM * sizeof(int));
	if(NULL == ps32local)
	{
		fprintf(stderr, "Error malloc()\n");
		exit(EXIT_FAILURE);
	}

	rnd_populate(ps32local, SIZE_SHARE_MEM);


#ifdef ENABLE_STAGE_OUTPUT
	printf("Step 1: Generate Random Array Finished!\n");
#endif

	/* create the shared memory segment */
	s32shm_fd = shm_open(getShareMemName(), O_CREAT | O_RDWR, S_IRWXU);

	/* configure the size of the shared memory segment */
	s32ret = ftruncate(s32shm_fd, sizeof(shm_data));
	if(s32ret < 0)
	{
		fprintf(stderr, "Error Ftruncate()\n");
		exit(EXIT_FAILURE);
	}

	/* now map the shared memory segment in the address space of the process */
	ptr = mmap( NULL,
				sizeof(shm_data),
				PROT_READ | PROT_WRITE,
				MAP_SHARED,
				s32shm_fd,
				0);
	pstrshm = (shm_data*)ptr;
	if (ptr == MAP_FAILED) {
		fprintf(stderr, "Map failed\n");
		exit(EXIT_FAILURE);
	}

	/* Leader: Write Guard */
	pstrshm->s32write_guard = 0;
#ifdef ENABLE_STAGE_OUTPUT
	printf("Step 2: Mapping Finished!\n");
#endif
	while(0 == pstrshm->s32write_guard)
	{
	}
	
	
	/* Copy the array to shared mem */
	memcpy( (void*)pstrshm->as32data,
			(void*)ps32local,
			sizeof(int) * SIZE_SHARE_MEM);

	/* Notify Follower to read */
#ifdef ENABLE_STAGE_OUTPUT
	printf("Step 2: Copying Finished! Wait to exit!\n");
#endif
	pstrshm->s32read_guard   = 1;
	pstrshm->s32delete_guard = 0;
	while(0 == pstrshm->s32delete_guard)
	{
	}
	

	/* Print Local Data */
#ifndef STUDIO15_Q6
	print_intlist(ps32local, SIZE_SHARE_MEM);
#endif

	munmap(ptr, sizeof(shm_data));
	shm_unlink(getShareMemName());
	free(ps32local);

#ifdef ENABLE_STAGE_OUTPUT
	printf("Step 3: Successfully Exit!\n");
#endif

	return 0;
}
