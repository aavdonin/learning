all: server select_server poll_server epoll_server pool_server

server: server.o serv_tcp.o serv_udp.o cl_handler.o accept.o
	gcc server.o serv_tcp.o serv_udp.o cl_handler.o accept.o -lpthread -o ../bin/server

server.o: server.c ../defines.h serv_tcp.h serv_udp.h cl_handler.h
	gcc -c server.c

serv_tcp.o: serv_tcp.c ../defines.h
	gcc -c serv_tcp.c

serv_udp.o: serv_udp.c ../defines.h
	gcc -c serv_udp.c

cl_handler.o: cl_handler.c ../defines.h
	gcc -c cl_handler.c

select_server: select_server.o cl_handler.o accept.o
	gcc select_server.o cl_handler.o accept.o -o ../bin/select_server

select_server.o: select_server.c ../defines.h accept.h
	gcc -c select_server.c

accept.o: accept.c ../defines.h
	gcc -c accept.c

poll_server: poll_server.o cl_handler.o accept.o
	gcc poll_server.o cl_handler.o accept.o -o ../bin/poll_server

poll_server.o: poll_server.c ../defines.h accept.h
	gcc -c poll_server.c

epoll_server: epoll_server.o cl_handler.o accept.o
	gcc epoll_server.o cl_handler.o accept.o -o ../bin/epoll_server

epoll_server.o: epoll_server.c ../defines.h accept.h
	gcc -c epoll_server.c

pool_server: pool_server.o cl_handler.o accept.o
	gcc pool_server.o cl_handler.o accept.o -lpthread -o ../bin/pool_server

pool_server.o: pool_server.c ../defines.h accept.h
	gcc -c pool_server.c

