#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <errno.h>


int main(int argc, char* argv[])
{
	uid_t s32uid;
	int s32retval;
	//s32uid = getuid();
	s32uid = (uid_t)syscall(__NR_getuid);
	printf("Now, UID = %d\n", s32uid);
	s32retval = (int)syscall(__NR_setuid,0);
	if (0 != s32retval)
	{
		printf("Error to setuid() failed! Reason: %s\n", strerror(errno));	
	}	
	s32uid = (uid_t)syscall(__NR_getuid);
	printf("After, UID = %d\n", s32uid);
	return 0;
}
