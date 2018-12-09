all: client

client: client.o cli_tcp.o cli_udp.o
	gcc client.o cli_tcp.o cli_udp.o -o ../bin/client

client.o: client.c ../defines.h cli_tcp.h cli_udp.h
	gcc -c client.c

cli_tcp.o: cli_tcp.c ../defines.h
	gcc -c cli_tcp.c

cli_udp.o: cli_udp.c ../defines.h
	gcc -c cli_udp.c
