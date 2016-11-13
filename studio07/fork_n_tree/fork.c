#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char* argv[])
{
	pid_t s32pid;
	printf("[Parent] Ready to fork!\n");

	s32pid = fork();
	if(0 == s32pid)
	{
		// Child
		printf("[Child ] I am his son.\n");
	}
	else
	{
		// parent
		printf("[Parent] Child PID = %d\n", s32pid);
	}
	
	return 0;
}
