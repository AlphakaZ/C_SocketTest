#include <stdlib.h> //atoi(), exit(), EXIT_FAILURE, EXIT_SUCCESS

#define MSGSIZE 1024
#define BUFSIZE (MSGSIZE + 1)

#include "st_client.h"

int main(int argc, char* argv[]){
    
    ClientModule cMdl;
    // int ret=0;

    setupClient(argc, argv, &cMdl);
    return runClient(&cMdl);
    // closeClient(&cMdl);

    // return ret;
}