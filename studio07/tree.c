#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char* argv[])
{
	pid_t s32pid1;
	pid_t s32pid2;
	int s32Level;
	char pcLevel[5] = "";

	if(2 != argc)
	{
		printf("Argument Error!\n");
		exit(EXIT_FAILURE);
	}

	s32Level = atoi(argv[1]);
	//printf("%d\n", s32Level);

	if(s32Level > 0)
	{

		s32pid1 = fork();
		if(0 == s32pid1)
		{
			// Child
			sprintf(pcLevel, "%d", s32Level - 1);
			if(0 > execlp("./tree", "tree", pcLevel, NULL))
			{
				printf("Execlp Error!\n");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
		}

		s32pid2 = fork();
		if(0 == s32pid2)
		{
			// Child
			sprintf(pcLevel, "%d", s32Level - 1);
			if(0 > execlp("./tree", "tree", pcLevel, NULL))
			{
				printf("Execlp Error!\n");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
		}

	}

	if(s32Level > 0)
	{
		printf("Level[%d] Parent:PID[%6d] Children [%6d], [%6d]\n", s32Level , getpid(), s32pid1, s32pid2);
	}
	else
	{
		printf("Level[%d] Parent:PID[%6d] No Children\n", s32Level, getpid());
	}

	sleep(120);
	
	return 0;
}
