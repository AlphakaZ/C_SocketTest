#ifndef __ST_SERVER_H__
#define __ST_SERVER_H__

#include <arpa/inet.h> // struct sockaddr_in, struct sockaddr

// 呼び出し側で定義されていなければ定義

#ifndef QUEUELIMIT
#define QUEUELIMIT 5
#endif

#ifndef MSGSIZE
#define MSGSIZE 1024
#endif

#ifndef BUFSIZE
#define BUFSIZE (MSGSIZE + 1)
#endif


typedef struct{
    int servSock; //server socket descripter
    int clitSock; //client socket descripter
    struct sockaddr_in servSockAddr; //server internet socket address
    struct sockaddr_in clitSockAddr; //client internet socket address
    unsigned short servPort; //server port number

}ServerModule;

void setupServer(int argc, char** argv, ServerModule* sMdl);
void runServer(ServerModule* sMdl);
void closeServer(ServerModule* sMdl);

#endif