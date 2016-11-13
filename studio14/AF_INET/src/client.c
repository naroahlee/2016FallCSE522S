#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <netdb.h>

#define HOST_IP "71.14.185.58"
/* #define HOST_IP "127.0.0.1" */
#define PORT_NO (17000)

int main(int argc, char* argv[])
{
	int s32rdfd;
	char cbuf;
	struct sockaddr_in straddr;
    struct hostent *server;

	/* Create Socket FD */
	s32rdfd = socket(AF_INET, SOCK_STREAM, 0);
	if (s32rdfd < 0) 
	{		
		perror("ERROR opening socket");
		exit (EXIT_FAILURE);
	}


    server = gethostbyname(HOST_IP);
    if (server == NULL) 
	{
        perror("ERROR, no such host.");
		exit (EXIT_FAILURE);
    }

	/* Bind a name to the socket. */
    bzero((char *) &straddr, sizeof(straddr));
    straddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
          (char *)&straddr.sin_addr.s_addr,
          server->h_length);
    straddr.sin_port = htons(PORT_NO);

    if (connect(s32rdfd, (struct sockaddr *)&straddr,sizeof(straddr)) < 0) 
	{
        perror("ERROR connecting");
		exit (EXIT_FAILURE);
	}

	/* '\0' matters */
	while(read(s32rdfd, &cbuf, sizeof(char) > 0))
	{
		printf("%c", cbuf);
	}
	printf("\n");

	close(s32rdfd);

	return 0;
}
