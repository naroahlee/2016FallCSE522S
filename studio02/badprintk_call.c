#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <asm/unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <errno.h>


int main(int argc, char* argv[])
{
	uid_t s32uid;
	int s32retval;
	s32retval = (int)syscall(392, 10);
	//s32retval = (int)badprintk(10);
	if (0 != s32retval)
	{
		printf("ERROR: sys_badprintk() failed! Reason: %s\n", strerror(errno));	
	}	
	printf("Exit.\n");
	return 0;
}
