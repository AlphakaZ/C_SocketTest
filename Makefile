all: tcpd tcpc

tcpd: src/tcpd.c
	gcc -Wall -o tcpd src/tcpd.c

tcpc: src/tcpc.c
	gcc -Wall -o tcpc src/tcpc.c







.PHONY: clean
clean:
	rm -f tcpd tcpc build/*.o
