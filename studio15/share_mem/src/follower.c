#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "shm_ds.h"

int main(int argc, char* argv[])
{
	int s32shm_fd;
	void *ptr;
	shm_data *pstrshm;
	int* ps32local;

	ps32local = (int*)malloc(SIZE_SHARE_MEM * sizeof(int));
	if(NULL == ps32local)
	{
		fprintf(stderr, "Error malloc()\n");
		exit(EXIT_FAILURE);
	}
	/* create the shared memory segment */
	/* No O_CREAT Here */
	s32shm_fd = shm_open(getShareMemName(), O_RDWR, S_IRWXU);

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

	pstrshm->s32write_guard = 1;
	pstrshm->s32read_guard = 0;
	while(0 == pstrshm->s32read_guard)
	{
	}

	/* print_intlist(pstrshm->as32data, SIZE_SHARE_MEM); */
#ifndef STUDIO15_Q6
	print_intlist(pstrshm->as32data, SIZE_SHARE_MEM);
	
#else
	/* Q5 */
	memcpy( (void*)ps32local,
			(void*)pstrshm->as32data,
			sizeof(int) * SIZE_SHARE_MEM);
#endif

	pstrshm->s32delete_guard = 1;

	munmap(ptr, sizeof(shm_data));
	shm_unlink(getShareMemName());
	free(ps32local);
	
	return 0;
}
