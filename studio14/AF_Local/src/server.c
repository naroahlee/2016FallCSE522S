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
#define CONNECT_RETRY (5)
#define BUF_LEN (1024)

int main(int argc, char* argv[])
{
	int s32i;
	int s32idx;
	int s32listenfd;
	int s32newskfd;
	int s32size;
	socklen_t s32len;
	char *pcBuf;
	int s32stringlen;
	struct sockaddr_un straddr;

	/* Create Socket FD */
	s32listenfd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (s32listenfd < 0) 
	{		
		perror("ERROR opening socket");
		exit (EXIT_FAILURE);
	}

	/* Bind a name to the socket. */
	straddr.sun_family = AF_LOCAL;
	strncpy (straddr.sun_path, FILE_PATH, sizeof(straddr.sun_path));
	straddr.sun_path[sizeof(straddr.sun_path) - 1] = '\0';

/* The size of the address is
   the offset of the start of the filename,
   plus its length (not including the terminating null byte).
   Alternatively you can just do:
   size = SUN_LEN (&name);
*/
	s32size = SUN_LEN(&straddr);
	if (bind (s32listenfd, (struct sockaddr *)&straddr, s32size) < 0)
	{
		perror ("bind");
		exit (EXIT_FAILURE);
	}

	listen(s32listenfd, 5);

	s32stringlen = BUF_LEN;
	pcBuf = (char *)malloc(sizeof(char) * s32stringlen);
	if(NULL == pcBuf )
	{
		perror ("malloc");
		exit (EXIT_FAILURE);
	}
	

	while(1)
	{

		s32len = sizeof(straddr);
		
		for(s32i = 0; s32i < CONNECT_RETRY; s32i++)
		{
			s32newskfd = accept(s32listenfd, 
							   (struct sockaddr *)&straddr,
							   &s32len);
			if(s32newskfd < 0)
			{
				perror("ERROR Connection socket: Retry!");
				continue;
			}
			else
			{
				break;
			}
		}
		if(s32newskfd < 0)
		{
			perror("ERROR: Retry Time over!");
			exit (EXIT_FAILURE);
		}

		s32idx = 0;
		while(read(s32newskfd, &pcBuf[s32idx++], sizeof(char)) > 0)
		{
			if(s32idx >= s32stringlen)
			{
				s32stringlen += BUF_LEN;
				pcBuf = realloc(pcBuf, sizeof(char) * s32stringlen);
				if(NULL == pcBuf)
				{
					perror ("realloc");
					exit (EXIT_FAILURE);
				}
			}
		}

		printf("[Rcv Msg] %s\n", pcBuf);
		close(s32newskfd);

		if(0 == strncmp("quit", pcBuf, 4))
		{
			break;
		}
	}


	
	free(pcBuf);
	close(s32listenfd);
	
	unlink(FILE_PATH);
	return 0;
}
