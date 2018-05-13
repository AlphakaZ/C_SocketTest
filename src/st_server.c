#include "st_server.h"

#include <stdio.h> //printf(), fprintf(), perror()

#include <sys/socket.h> //socket(), bind(), accept(), listen()
#include <sys/fcntl.h>
#include <sys/types.h>

#include <arpa/inet.h> // struct sockaddr_in, struct sockaddr, inet_ntoa()

#include <stdlib.h> //atoi(), exit(), EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> //memset()
#include <unistd.h> //close()

#include <netinet/in.h>
#include <netdb.h>

#include <stdbool.h>

#define DOCUMENT_ROOT "./html/"
#define FILEPATH_LENGTH 256


// サーバの実処理はここに記述される

static void initServer(ServerModule *s){
    //何もしない
}

static void argCheck(int argc){
    if ( argc != 2) {//引数のチェック
        fprintf(stderr, "argument count mismatch error.\n");
        exit(EXIT_FAILURE);
    }
}

static int getServPortNumber(char* str)
{
    int n = (unsigned short) atoi(str);
    if(n == 0){
        fprintf(stderr, "invalid port number.\n");
        exit(EXIT_FAILURE);        
    }
    return n;
}

static int genServSocket()
{
    int n = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(n < 0){
        perror("socket() failed.");
        exit(EXIT_FAILURE);
    }
    return n;
}

static void initServSockAddr(ServerModule* sMdl){
    memset(&(sMdl->servSockAddr),0,sizeof(sMdl->servSockAddr));
    sMdl->servSockAddr.sin_family      = AF_INET;// AF_INET = IPv4
    sMdl->servSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);// 
    sMdl->servSockAddr.sin_port        = htons(sMdl->servPort);//ポート番号の格納 htonl(),htons()はネットワークに対応するエンディアン変換関数
}

static void bindSocketWithAddress(ServerModule* sMdl){
    if (bind(sMdl->servSock, (struct sockaddr *) &(sMdl->servSockAddr), sizeof(sMdl->servSockAddr) ) < 0 ) {
        perror("bind() failed.");
        exit(EXIT_FAILURE);
    }
}

static void readyServer(ServerModule* sMdl){
    if (listen(sMdl->servSock, QUEUELIMIT) < 0) {
        perror("listen() failed.");
        exit(EXIT_FAILURE);
    }
}

// セットアップ関数
void setupServer(int argc, char** argv, ServerModule* sMdl){
    initServer(sMdl);
    argCheck(argc);

    // 文字列になっているポート番号を整数に変換する
    sMdl->servPort = getServPortNumber(argv[1]);
    // socket(プロトコルファミリ,  ソケットの種類, プロトコル)は、OSにソケットの作成を依頼するシステムコール
    sMdl->servSock = genServSocket();

    //サーバ側の構造体を初期化
    initServSockAddr(sMdl);
    
    //作成したソケットにIPアドレスとポート番号を紐づける
    bindSocketWithAddress(sMdl);

    //ここで初めて、クライアントからの接続状態を受け付ける状態になる
    readyServer(sMdl);
}

void returnString(const char* str, ServerModule* sMdl)
{
	char recvBuffer[BUFSIZE];
	int recvMsgSize,sendMsgSize;
    
    while(1) {
	    if ((recvMsgSize = recv(sMdl->clitSock, recvBuffer, BUFSIZE, 0)) < 0) {
	        perror("recv() failed.");
	        exit(EXIT_FAILURE);
	    } else if(recvMsgSize == 0){
	        fprintf(stderr, "connection closed by foreign host.\n");
	        break;
	    }

	    strcpy(recvBuffer,str);//固定文字列
	    recvMsgSize = strlen(str);

	    if((sendMsgSize = send(sMdl->clitSock, recvBuffer, recvMsgSize, 0)) < 0){
	        perror("send() failed.");
	        exit(EXIT_FAILURE);
	    } else if(sendMsgSize == 0){
	        fprintf(stderr, "connection closed by foreign host.\n");
	        break;
	    }
	}
	close(sMdl->clitSock);
}

/**
 * メッセージ送信用関数
 *
 * @param fd   ソケットディスクリプタ
 * @param *msg 送信するメッセージ
 */
static int send_msg(int fd, char *msg) {
    int len;
    len = strlen(msg);

    // 指定されたメッセージ`msg`をソケットに送信
    if (write(fd, msg, len) != len) {
        fprintf(stderr, "error: writing.");
    }

    return len;
}

static bool isHttpFile(const char* filename){
	char* file_ex = strstr(filename,".");
	
	if(file_ex == NULL)return false;

	if(strcmp(file_ex,".html")==0 || strcmp(file_ex,".htm")==0){
		return true;
	}

	return false;
}

// Todo: 関数ポインタで処理を渡せるようにする
static void http(ServerModule* sMdl)
{
	int clitSock = sMdl->clitSock;

	// int len;		//ファイル読み込み時のバイトサイズ格納
	int read_fd=-1;	//要求ファイルを開くためのファイルディスクリプタ
	char buf[BUFSIZE];//ソケットからデータを読み取るバッファ
	char method[16];//メソッド名を格納(GETとか)
	char uri_addr[FILEPATH_LENGTH];//接続相手のアドレス
	char http_ver[64];
	
	char* uri_file;// 接続要求のあったファイル名
	// strcat(uri_file,DOCUMENT_ROOT);

	if(read(clitSock, buf, BUFSIZE) <= 0){//readは、第4引数が0のrecvと同等
		fprintf(stderr, "error: reading a request.\n");
		exit(EXIT_FAILURE);
	}

	sscanf(buf, "%s %s %s", method, uri_addr, http_ver);
	if (strcmp(method, "GET") != 0) {
		fprintf(stderr, "error: Method is no GET");
        send_msg(clitSock, "501 Not implemented.");
        close(clitSock);
        return;
    }

	uri_file = uri_addr + 1;//行頭の/を取り除く

	if(!isHttpFile(uri_file)){
		fprintf(stderr, "error: The file is not http file.");
		close(clitSock);
		return;
	}

	//ここでパスを取得する必要がある
	char filepath[FILEPATH_LENGTH];
	snprintf(filepath,FILEPATH_LENGTH,"%s%s",DOCUMENT_ROOT,uri_file);
	
	if((read_fd = open(filepath, O_RDONLY, 0666)) == -1){
		send_msg(clitSock, "404 Not Found");
		printf("404 error: %s",filepath);
		close(clitSock);
		return;
	}

	//HTTPヘッダを送信
	send_msg(clitSock, "HTTP/1.0 200 OK\r\n");
	send_msg(clitSock, "Content-Type: text/html\r\n");
	send_msg(clitSock, "\r\n");

	// ファイルポインタを使ってファイルを読み込み、書き出す
	write(clitSock, uri_addr,strlen(uri_addr));
	printf("file: %s\n",filepath);
	// FILE *fp;

	close(clitSock);
}

// 動作関数
void runServer(ServerModule* sMdl)
{
	unsigned int clitLen; // client internet socket address length

    while(1) {
        clitLen = sizeof(sMdl->clitSockAddr);
        if ((sMdl->clitSock = accept(sMdl->servSock, (struct sockaddr *) &(sMdl->clitSockAddr), &clitLen)) < 0) {
            perror("accept() failed.");
            exit(EXIT_FAILURE);
        }else{
        	//ここで通信処理

        	printf("connected from %s.\n", inet_ntoa(sMdl->clitSockAddr.sin_addr));
        	// 常に"Return"と返却する
        	// returnString("Return\n",sMdl);
        	http(sMdl);
        }
    }
}

void closeServer(ServerModule* sMdl){
    close(sMdl->servSock);
}