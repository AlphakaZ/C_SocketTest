#ifndef __ST_CLIENT_H__
#define __ST_CLIENT_H__

#include <arpa/inet.h> // struct sockaddr_in, struct sockaddr

// 呼び出し側で定義されていなければ定義

#ifndef MSGSIZE
#define MSGSIZE 1024
#endif

#ifndef BUFSIZE
#define BUFSIZE (MSGSIZE + 1)
#endif

typedef struct
{
	int sock;
	struct sockaddr_in servSockAddr;
	unsigned short servPort;

}ClientModule;

void setupClient(int argc, char** argv, ClientModule* cMdl);
int runClient(ClientModule* cMdl);
// void closeClient(ClientModule* cMdl);

#endif