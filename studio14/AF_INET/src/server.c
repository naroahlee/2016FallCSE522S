#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define CONNECT_RETRY (5)

#define PORT_NO (17000)

int main(int argc, char* argv[])
{
	int s32i;
	int s32ret;
	int s32listenfd;
	int s32stringlen;
	int s32newskfd;
	int s32optval;
	socklen_t s32optlen;
	socklen_t s32len;
	char acBuf[250] = "This is a test message.";
	struct sockaddr_in straddr;

	/* Create Socket FD */
	s32listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (s32listenfd < 0) 
	{		
		perror("ERROR opening socket");
		exit (EXIT_FAILURE);
	}

	/* Bind a name to the socket. */
	bzero((char *) &straddr, sizeof(straddr));
	straddr.sin_family      = AF_INET;
	straddr.sin_addr.s_addr = INADDR_ANY;
	straddr.sin_port        = htons(PORT_NO);

	/* Avoid Port already in use problem */
	s32optval = 1;
	s32optlen = sizeof(int);
	setsockopt(s32listenfd, SOL_SOCKET, SO_REUSEADDR, &s32optval, s32optlen);

	/* Using a long-live TCP connection */
	s32optval = 1;
	s32optlen = sizeof(int);
	setsockopt(s32listenfd, SOL_SOCKET, SO_KEEPALIVE, &s32optval, s32optlen);

	/* Bind the serv_addr + port with the dedicate socket */
	if (bind(s32listenfd, (struct sockaddr *) &straddr,
		  sizeof(straddr)) < 0) 
	{
		perror("ERROR on binding");
		return -1;
	}
	/* Listen and setup the outstanding connect request */
	listen(s32listenfd, 5);


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

		printf("Connected!\n");

		s32stringlen = strlen(acBuf) + 1;	
		s32ret = write(s32newskfd, acBuf, sizeof(char) * s32stringlen);
		if (s32ret != s32stringlen) 
		{		
			perror("ERROR read");
			exit (EXIT_FAILURE);
		}

		close(s32newskfd);
	}

	close(s32listenfd);

	return 0;
}
