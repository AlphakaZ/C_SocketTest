all: tcpd tcpc

# サーバ側

tcpd: src/tcpd.c st_server.o
	gcc -Wall -o tcpd src/tcpd.c st_server.o

st_server.o: src/st_server.c
	gcc -Wall -c src/st_server.c

st_server.o: src/st_server.h


# クライアント側	

tcpc: src/tcpc.c st_client.o
	gcc -Wall -o tcpc src/tcpc.c st_client.o

st_client.o: src/st_client.c
	gcc -Wall -c src/st_client.c

st_client.o: src/st_client.h







.PHONY: clean
clean:
	rm -f tcpd tcpc build/*.o
