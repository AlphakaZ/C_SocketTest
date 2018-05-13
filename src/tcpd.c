// 参考・引用：https://qiita.com/tajima_taso/items/2f0606db7764580cf295

#include <stdlib.h> //atoi(), exit(), EXIT_FAILURE, EXIT_SUCCESS

#define QUEUELIMIT 5
#define MSGSIZE 1024
#define BUFSIZE (MSGSIZE + 1)

#include "st_server.h"

int main(int argc, char* argv[]) {

    ServerModule sMdl;

    setupServer(argc, argv, &sMdl);
    runServer(&sMdl);
    closeServer(&sMdl);

    return EXIT_SUCCESS;
}