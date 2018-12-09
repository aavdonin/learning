all: client server

server: server.o serv_tcp.o serv_udp.o
	gcc server.o serv_tcp.o serv_udp.o -lpthread -o server

client: client.o cli_tcp.o cli_udp.o
	gcc client.o cli_tcp.o cli_udp.o -o client

server.o: server.c defines.h serv_tcp.h serv_udp.h
	gcc -c server.c

serv_tcp.o: serv_tcp.c defines.h
	gcc -c serv_tcp.c

serv_udp.o: serv_udp.c defines.h
	gcc -c serv_udp.c

client.o: client.c defines.h cli_tcp.h cli_udp.h
	gcc -c client.c

cli_tcp.o: cli_tcp.c defines.h
	gcc -c cli_tcp.c

cli_udp.o: cli_udp.c defines.h
	gcc -c cli_udp.c

clean:
	rm *.o client server
