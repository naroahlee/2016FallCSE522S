#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sched.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#define MAX_CORE_NO     (4)
#define IRT_NUM (500000000)

int main(int argc, char *argv[])
{
    sigset_t allsigs;
    int cur_val = 0;
	int s32CPUNo = 0;
	int s32i = 0;

	for(s32i = 0; s32i < IRT_NUM; s32i++)
	{
	}
    
    exit(EXIT_SUCCESS);
}

