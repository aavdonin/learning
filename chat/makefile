all: server client

server: server.o folders
	gcc server/server.o -o bin/server

server.o: server/server.c defines.h
	gcc -c server/server.c -o server/server.o

client: client/client.o client/ui.o folders
	gcc client/client.o client/ui.o -lncurses -lpthread -o bin/client

client.o: client/client.c defines.h
	gcc -c client/client.c -o client/client.o

ui.o: client/ui.c defines.h
	gcc -c client/ui.c -o client/ui.o

clean:
	rm -rf bin */*.o *.pipe

folders:
	mkdir -p bin

debug: debug_server debug_client

debug_server: server.c
	gcc -g server/server.c -o bin/server

debug_client: client.c
	gcc -g client/client.c -lncurses -lpthread -o bin/client