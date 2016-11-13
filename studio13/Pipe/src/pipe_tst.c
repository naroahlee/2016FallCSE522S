#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char* argv[])
{
	int as32pipefd[2];
	const char acMsg1[] = "[Tst Msg1] Today\n";
	const char acMsg2[] = "[Tst Msg2] I am\n";
	const char acMsg3[] = "[Tst Msg3] happy.\n";
	char cbuf;
	pid_t s32childpid;

	if(0 != pipe(as32pipefd))
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	s32childpid = fork();
	if(0 == s32childpid)
	{
		close(as32pipefd[0]);
		write(as32pipefd[1], acMsg1, strlen(acMsg1));
		write(as32pipefd[1], acMsg2, strlen(acMsg2));
		write(as32pipefd[1], acMsg3, strlen(acMsg3));
		close(as32pipefd[1]);
	}
	else
	{
		close(as32pipefd[1]);
		while(read(as32pipefd[0], &cbuf, 1) > 0)
		{
			write(STDOUT_FILENO, &cbuf, 1);
		}
		close(as32pipefd[0]);
		exit(EXIT_SUCCESS);
	}

	return 0;
}
