#ifndef __ST_NETWORK_H__
#define __ST_NETWORK_H__

#include <stdio.h> //printf(), fprintf(), perror()
#include <sys/socket.h> //socket(), bind(), accept(), listen()
#include <arpa/inet.h> // struct sockaddr_in, struct sockaddr, inet_ntoa()
#include <stdlib.h> //atoi(), exit(), EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> //memset()
#include <unistd.h> //close()

#define QUEUELIMIT 5
#define MSGSIZE 1024
#define BUFSIZE (MSGSIZE + 1)

typedef struct{
    int servSock; //server socket descripter
    int clitSock; //client socket descripter
    struct sockaddr_in servSockAddr; //server internet socket address
    struct sockaddr_in clitSockAddr; //client internet socket address
    unsigned short servPort; //server port number
    unsigned int clitLen; // client internet socket address length
    char recvBuffer[BUFSIZE];
    int recvMsgSize, sendMsgSize;
}ServerModule;

void initServer(ServerModule)

#endif
//__ST_NETWORK_H__
