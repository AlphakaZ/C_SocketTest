// 参考・引用：https://qiita.com/tajima_taso/items/2f0606db7764580cf295

#include <stdio.h> //printf(), fprintf(), perror()
#include <sys/socket.h> //socket(), bind(), accept(), listen()
#include <arpa/inet.h> // struct sockaddr_in, struct sockaddr, inet_ntoa()
#include <stdlib.h> //atoi(), exit(), EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> //memset()
#include <unistd.h> //close()

#define QUEUELIMIT 5
#define MSGSIZE 1024
#define BUFSIZE (MSGSIZE + 1)

void l2b(char* str){
    for(int i=0;i<=strlen(str);i++){
    /* アルファベットの小文字なら変換 */
    if(str[i]>=97&&str[i]<=122)
      str[i]=str[i]-32;
  }
}

int main(int argc, char* argv[]) {

    int servSock; //server socket descripter
    int clitSock; //client socket descripter
    struct sockaddr_in servSockAddr; //server internet socket address
    struct sockaddr_in clitSockAddr; //client internet socket address
    unsigned short servPort; //server port number
    unsigned int clitLen; // client internet socket address length
    char recvBuffer[BUFSIZE];
    int recvMsgSize, sendMsgSize;


    if ( argc != 2) {//引数のチェック
        fprintf(stderr, "argument count mismatch error.\n");
        exit(EXIT_FAILURE);
    }
    // 文字列になっているポート番号を整数に変換する
    if ((servPort = (unsigned short) atoi(argv[1])) == 0) {
        fprintf(stderr, "invalid port number.\n");
        exit(EXIT_FAILURE);
    }
    // socket(プロトコルファミリ,  ソケットの種類, プロトコル)は、OSにソケットの作成を依頼するシステムコール
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 ){
        perror("socket() failed.");
        exit(EXIT_FAILURE);
    }
    //サーバ側の構造体を初期化
    memset(&servSockAddr, 0, sizeof(servSockAddr));
    servSockAddr.sin_family      = AF_INET;// AF_INET = IPv4
    servSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);// 
    servSockAddr.sin_port        = htons(servPort);//ポート番号の格納 htonl(),htons()はネットワークに対応するエンディアン変換関数
    
    //作成したソケットにIPアドレスとポート番号を紐づける
    if (bind(servSock, (struct sockaddr *) &servSockAddr, sizeof(servSockAddr) ) < 0 ) {
        perror("bind() failed.");
        exit(EXIT_FAILURE);
    }
    //ここで初めて、クライアントからの接続状態を受け付ける状態になる
    if (listen(servSock, QUEUELIMIT) < 0) {
        perror("listen() failed.");
        exit(EXIT_FAILURE);
    }

    while(1) {
        clitLen = sizeof(clitSockAddr);
        if ((clitSock = accept(servSock, (struct sockaddr *) &clitSockAddr, &clitLen)) < 0) {
            perror("accept() failed.");
            exit(EXIT_FAILURE);
        }

        printf("connected from %s.\n", inet_ntoa(clitSockAddr.sin_addr));
        while(1) {
            if ((recvMsgSize = recv(clitSock, recvBuffer, BUFSIZE, 0)) < 0) {
                perror("recv() failed.");
                exit(EXIT_FAILURE);
            } else if(recvMsgSize == 0){
                fprintf(stderr, "connection closed by foreign host.\n");
                break;
            }
            
            if((sendMsgSize = send(clitSock, recvBuffer, recvMsgSize, 0)) < 0){
                perror("send() failed.");
                exit(EXIT_FAILURE);
            } else if(sendMsgSize == 0){
                fprintf(stderr, "connection closed by foreign host.\n");
                break;
            }
        }
        close(clitSock);
    }

    close(servSock);

    return EXIT_SUCCESS;
}