#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "shm_ds.h"

char gacshmname[] = NAME_SHARE_MEM;
char* getShareMemName(void)
{
	return gacshmname;
}

void rnd_populate(int* ps32ptr, int s32len)
{
	int s32i;

    srand((unsigned)clock());
	for(s32i = 0; s32i < s32len; s32i++)
	{
		ps32ptr[s32i] = rand() % 1000;
	}
	return;
}

void print_intlist(volatile int* ps32ptr, int s32len)
{
	int s32i;
	for(s32i = 0; s32i < s32len; s32i++)
	{
		printf("[%5d] ", ps32ptr[s32i]);
		if(0 == (s32i + 1) % 5)
		{
			printf("\n");
		}
	}
	return;
}
