#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define FILE_PATH "./temp_socket"

int main(int argc, char* argv[])
{
	int s32ret;
	int s32wrfd;
	int s32stringlen;
	char acBuf[256];
	struct sockaddr_un straddr;

	printf("Input Test Msg:");
	gets(acBuf);

	/* Create Socket FD */
	s32wrfd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (s32wrfd < 0) 
	{		
		perror("ERROR opening socket");
		exit (EXIT_FAILURE);
	}

	/* Bind a name to the socket. */
	straddr.sun_family = AF_LOCAL;
	strncpy (straddr.sun_path, FILE_PATH, sizeof(straddr.sun_path));
	straddr.sun_path[sizeof(straddr.sun_path) - 1] = '\0';

    if (connect(s32wrfd, (struct sockaddr *)&straddr,sizeof(straddr)) < 0) 
	{
        perror("ERROR connecting");
	}

	
	/* '\0' matters */
	s32stringlen = strlen(acBuf) + 1;
	s32ret = write(s32wrfd, acBuf, sizeof(char) * (s32stringlen + 1));
	if (s32ret != s32stringlen + 1) 
	{		
		perror("ERROR write");
		exit (EXIT_FAILURE);
	}

	close(s32wrfd);

	return 0;
}
