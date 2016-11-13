/* network_util.h							 */
/* Provide interface to HMM and DMM		     */
/* Thus I can easily handle the socket thing */

#ifndef __HDMM_NETWORK_UTIL__
#define __HDMM_NETWORK_UTIL__

void error(const char *msg);
int createSrv(void*(pfxnSrv)(void *), int s32portno,
			  int* ps32retfd, pthread_t* ppthSrv, int s32connNum);
int destroySrv(int sockfd, pthread_t pthSrv);
int connectServer(const char* pcPeerIP, int s32portno);
int sendMsg(int s32skfd, void* pData, size_t s32size);
int recvMsg(int s32skfd, void* pData, size_t s32size);

#endif
