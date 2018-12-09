all: client server

server: server.o serv_tcp.o serv_udp.o
	gcc server.o serv_tcp.o serv_udp.o -lpthread -o server

client: client.o
	gcc client.o -o client

server.o: server.c defines.h serv_tcp.h serv_udp.h
	gcc -c server.c

serv_tcp.o: serv_tcp.c defines.h
	gcc -c serv_tcp.c

serv_udp.o: serv_udp.c defines.h
	gcc -c serv_udp.c

client.o: client.c
	gcc -c client.c

clean:
	rm *.o client server
