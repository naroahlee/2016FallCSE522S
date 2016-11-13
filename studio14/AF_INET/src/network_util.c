/* network_util.c */
/* I want to separate the module and make the code cleaner */
/* A simple server in the internet domain using TCP */
/* The port number is passed as an argument */
/* This server is served as a localmodeupdateserver */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include "updatepara.h"
#include "network_util.h"

/* Print Error */
void error(const char *msg)
{
    perror(msg);
    exit(1);
}
/* createSrv: create a pthread and bind a socket server to     */
/*			  someport in user-level						   */
/* Input : a server pointer to fnxn[pfxnSrv] to it             */
/*	       a dedicated port ID								   */
/* Output: a socket id,										   */
/*		   a pthread pointer			                       */
/* return: Successful(0) or Errorno(negative)				   */
int createSrv(void*(pfxnSrv)(void *), int s32portno,
			  int* ps32retfd, pthread_t* ppthSrv, int s32connNum)
{	
	pthread_t srv_thread;
	int s32skfd;
	int s32optval;
	socklen_t s32optlen;
	int s32ret;
	struct sockaddr_in serv_addr;

	/* Establish a socket: IPv4 + TCP */
	s32skfd = socket(AF_INET, SOCK_STREAM, 0);
	if (s32skfd < 0) 
	{		
		error("ERROR opening socket");
		return -1;
	}

	/* Setup the output							  */
	/* We will use this pointer to initial server */
	*ps32retfd = s32skfd;

	/* Clear and fill the serv_addr field */
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family      = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port        = htons(s32portno);

	/* Avoid Port already in use problem */
	s32optval = 1;
	s32optlen = sizeof(int);
	setsockopt(s32skfd, SOL_SOCKET, SO_REUSEADDR, &s32optval, s32optlen);

	/* Using a long-live TCP connection */
	s32optval = 1;
	s32optlen = sizeof(int);
	setsockopt(s32skfd, SOL_SOCKET, SO_KEEPALIVE, &s32optval, s32optlen);
	/* Bind the serv_addr + port with the dedicate socket */
	if (bind(s32skfd, (struct sockaddr *) &serv_addr,
		  sizeof(serv_addr)) < 0) 
	{
		error("ERROR on binding");
		return -1;
	}
	/* Listen and setup the outstanding connect request */
	listen(s32skfd, s32connNum);

	/* Bind the server fxn */
	s32ret = pthread_create(&srv_thread, 
					     NULL,
						 pfxnSrv,
						 (void *)ps32retfd);
	/* Block thread */
	// pthread_join(srv_thread, NULL);
	/* Setup the output */
	*ppthSrv = srv_thread;
	
	return s32ret;
}

/* destroySrv: destroy a pthread and bind a socket user-level  */
/* Input : a server pointer to fnxn[pfxnSrv] to it             */
/* Output: N/A							                       */
/* return: Successful(0) or Errorno(negative)				   */
int destroySrv(int sockfd, pthread_t pthSrv)
{
	close(sockfd);
	/* ATTENTION!!! */
	/* I am not sure whether the memory would be released before cancel */
	/* Potential BUG and Mem Leak exist */
	/* In Non-Shell Mode, a pthread_cancel is not always affective when */
	/* the H/DMM runs as a background process */
	pthread_kill(pthSrv, SIGKILL);
	//pthread_cancel(pthSrv);
	return 0;
}

/* connectServer: create a sk, bind a port of a host           */
/*			and send somedata to it							   */
/* Input : IP, portno							               */
/*		   pointer to data, size							   */
/* Output: N/A							                       */
/* return: Successful(0) or Errorno(negative)				   */
int connectServer(const char* pcPeerIP, int s32portno)
{
	int s32skfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    s32skfd = socket(AF_INET, SOCK_STREAM, 0);
    if (s32skfd < 0) 
        error("ERROR opening socket");

	/* Get Server by name/IP */
    server = gethostbyname(pcPeerIP);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

	/* Clear and bind the Socket */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(s32portno);

	/* Connect the socket */
    if (connect(s32skfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
	{
        error("ERROR connecting");
	}

	return s32skfd;
}

/* sendMsg: and send somedata to it							   */
/* Input : IP, portno							               */
/*		   pointer to data, size							   */
/* Output: N/A							                       */
/* return: Successful(0) or Errorno(negative)				   */
int sendMsg(int s32skfd, void* pData, size_t s32size)
{
	int n;
	
	/* Send the Register Info only once */
    n = send(s32skfd, (void *)pData, s32size, MSG_DONTWAIT);
    if (n < 0) 
         error("ERROR writing to socket");
	return 0;
}

/* recvMsg: accept a socket, bind with socked id		       */
/*			and recv somedata from it						   */
/* Input : s32skid								               */
/*		   pointer to data, size							   */
/* Output: N/A							                       */
/* return: newskfd											   */
/* Attention, you had to manually close the socket after using */
int recvMsg(int s32skfd, void* pData, size_t s32size)
{
	int s32rcvsize;

	/* I've got the connection */
	/* printf("Connnection Established!\n"); */
	/* Waiting to read */
	s32rcvsize = recv(s32skfd, pData, s32size, MSG_WAITALL);
	if (s32rcvsize < 0)
		error("ERROR reading from socket");

	return s32rcvsize;
}


