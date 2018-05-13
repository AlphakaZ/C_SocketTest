#include "st_client.h"

#include <stdio.h> //printf(), fprintf(), perror()
#include <sys/socket.h> //socket(), connect(), recv()
#include <arpa/inet.h> // struct sockaddr_in, struct sockaddr, inet_ntoa(), inet_aton()
#include <stdlib.h> //atoi(), exit(), EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> //memset()
#include <unistd.h> //close()

// クライアントの処理を記述する

static void argCheck(int argc){
    if ( argc != 3) {//引数のチェック
        fprintf(stderr, "argument count mismatch error.\n");
        exit(EXIT_FAILURE);
    }
}

static void initModule(const char* ipAddr, const char* port, ClientModule* cMdl){
	memset(&(cMdl->servSockAddr), 0, sizeof(cMdl->servSockAddr));
	cMdl->servSockAddr.sin_family = AF_INET;

	//指定したIPアドレスをバイナリ表現に変換
    if (inet_aton(ipAddr, &(cMdl->servSockAddr.sin_addr)) == 0) {
        fprintf(stderr, "Invalid IP Address.\n");
        exit(EXIT_FAILURE);
    }
    // ポート番号をバイナリ表現に変換
    if ((cMdl->servPort = (unsigned short) atoi(port)) == 0) {
        fprintf(stderr, "invalid port number.\n");
        exit(EXIT_FAILURE);
    }
    cMdl->servSockAddr.sin_port = htons(cMdl->servPort);
    // ソケットの作成をシステムに依頼
    if ((cMdl->sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 ){
        perror("socket() failed.");
        exit(EXIT_FAILURE);
    }
}

static void connectToServer(ClientModule* cMdl){
    // connect(ソケットディスクリプタ, サーバのIPアドレスとポート番号を含む構造体, 構造体サイズ)システムコールにより、接続を確立する
    // 3-wayハンドシェイクにより接続が確立するのはココ
    if (connect(cMdl->sock, (struct sockaddr*) &(cMdl->servSockAddr), sizeof(cMdl->servSockAddr)) < 0) {
        perror("connect() failed.");
        exit(EXIT_FAILURE);
    }
    printf("connect to %s\n", inet_ntoa(cMdl->servSockAddr.sin_addr));

}

void setupClient(int argc, char** argv, ClientModule* cMdl){

	// 引数の数をチェック
	argCheck(argc);

	initModule(argv[1], argv[2], cMdl);

	connectToServer(cMdl);
}

//改行文字を取り除く
static void lntrim(char *str) {  
  char *p;  
  p = strchr(str, '\n');  
  if(p != NULL) {  
    *p = '\0';  
  }  
}  

int runClient(ClientModule* cMdl){

	char recvBuffer[BUFSIZE]; //receive temporary buffer
    char sendBuffer[BUFSIZE];
    char httpedSendBuffer[BUFSIZE];

	while(1){
        // 入力待ち
        printf("please enter the filename:\t");
        if (fgets(sendBuffer, BUFSIZE, stdin) == NULL){
            fprintf(stderr, "invalid input string.\n");
            exit(EXIT_FAILURE);
        }
        lntrim(sendBuffer);
        sprintf(httpedSendBuffer, "GET %s HTTP/1.0/\r\n",sendBuffer);

        //システムコールsend()で、データをリモートホストに送信
        if (send(cMdl->sock, httpedSendBuffer, strlen(httpedSendBuffer), 0) <= 0) {
            perror("send() failed.");
            exit(EXIT_FAILURE);
        }
	    int byteRcvd  = 0;
	    int byteIndex = 0;
        while (byteIndex < MSGSIZE) {//文字はどんどん追加されてく
            // recv(接続が確立したソケットディスクリプタ, 受信メッセージのポインタ,受信メッセージ長さ,　ブロック動作の指定
            // 以上のシステムコールを用いて、システムの受信ストリームからバイト文字列を受信する。
            // *連続した文字列を受け取ることを前提にしているため、ナル文字は送受信に含まれないことに注意！
            // サーバからデータが送られてくる際にナル文字は含まない。
            byteRcvd = recv(cMdl->sock, &recvBuffer[byteIndex], 1, 0);//recvは、バッファに新たな文字列か追加されるまで停止する(1文字ずつ進む)
            if (byteRcvd > 0) {
                if (recvBuffer[byteIndex] == '\n'){//開業文字ならbreak
                    recvBuffer[byteIndex] = '\0';
                    if (strcmp(recvBuffer, "quit") == 0) {
                        close(cMdl->sock);
                        return EXIT_SUCCESS;
                    } else {
                        break;
                    }
                }
                // printf("%c\n",recvBuffer[byteIndex]);
                byteIndex += byteRcvd;
            } else if(byteRcvd == 0){
                perror("ERR_EMPTY_RESPONSE");
                exit(EXIT_FAILURE);
            } else {
                perror("recv() failed.");
                exit(EXIT_FAILURE);
            }
        }
        printf("server return: %s\n", recvBuffer);
    }
    return EXIT_SUCCESS;
}

// void closeClient(){
// 	//対称性を持たせるために配置
// 	return;
// }