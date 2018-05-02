#include <stdio.h> //printf(), fprintf(), perror()
#include <sys/socket.h> //socket(), connect(), recv()
#include <arpa/inet.h> // struct sockaddr_in, struct sockaddr, inet_ntoa(), inet_aton()
#include <stdlib.h> //atoi(), exit(), EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> //memset()
#include <unistd.h> //close()

#define MSGSIZE 1024
#define BUFSIZE (MSGSIZE + 1)

int main(int argc, char* argv[]){

    int sock; //local socket descripter
    struct sockaddr_in servSockAddr; //server internet socket address
    unsigned short servPort; //server port number
    char recvBuffer[BUFSIZE]; //receive temporary buffer
    char sendBuffer[BUFSIZE];

    if (argc != 3) { //引数チェック
        fprintf(stderr, "argument count mismatch error.\n");
        exit(EXIT_FAILURE);
    }
    // 構造体をクリア。構造体には接続先ホストのアドレス情報が格納されるため、詳細な代入は行わない
    memset(&servSockAddr, 0, sizeof(servSockAddr));
    //IPv4指定
    servSockAddr.sin_family = AF_INET;
    //指定したIPアドレスをバイナリ表現に変換
    if (inet_aton(argv[1], &servSockAddr.sin_addr) == 0) {
        fprintf(stderr, "Invalid IP Address.\n");
        exit(EXIT_FAILURE);
    }
    // ポート番号をバイナリ表現に変換
    if ((servPort = (unsigned short) atoi(argv[2])) == 0) {
        fprintf(stderr, "invalid port number.\n");
        exit(EXIT_FAILURE);
    }
    servSockAddr.sin_port = htons(servPort);
    // ソケットの作成を依頼
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 ){
        perror("socket() failed.");
        exit(EXIT_FAILURE);
    }
    // connect(ソケットディスクリプタ, サーバのIPアドレスとポート番号を含む構造体, 構造体サイズ)システムコールにより、接続を確立する
    // 3-wayハンドシェイクにより接続が確立するのはココ
    if (connect(sock, (struct sockaddr*) &servSockAddr, sizeof(servSockAddr)) < 0) {
        perror("connect() failed.");
        exit(EXIT_FAILURE);
    }

    printf("connect to %s\n", inet_ntoa(servSockAddr.sin_addr));

    while(1){
        // 入力待ち
        printf("please enter the characters:");
        if (fgets(sendBuffer, BUFSIZE, stdin) == NULL){
            fprintf(stderr, "invalid input string.\n");
            exit(EXIT_FAILURE);
        }
        //システムコールsend()で、データをリモートホストに送信
        if (send(sock, sendBuffer, strlen(sendBuffer), 0) <= 0) {
            perror("send() failed.");
            exit(EXIT_FAILURE);
        }

        int byteRcvd  = 0;
        int byteIndex = 0;
        while (byteIndex < MSGSIZE) {
            // recv(接続が確立したソケットディスクリプタ, 受信メッセージのポインタ,受信メッセージ長さ,　ブロック動作の指定
            // 以上のシステムコールを用いて、システムの受信ストリームからバイト文字列を受信する。
            byteRcvd = recv(sock, &recvBuffer[byteIndex], 1, 0);
            if (byteRcvd > 0) {
                if (recvBuffer[byteIndex] == '\n'){
                    recvBuffer[byteIndex] = '\0';
                    if (strcmp(recvBuffer, "quit") == 0) {
                        close(sock);
                        return EXIT_SUCCESS;
                    } else {
                        break;
                    }
                }
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