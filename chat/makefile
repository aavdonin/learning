all: server client

server: server.o array_fnc.o folders
	gcc server/server.o server/array_fnc.o -o bin/server

server.o: server/server.c server/array_fnc.h defines.h
	gcc -c server/server.c -o server/server.o

client: client/client.o client/ui.o folders
	gcc client/client.o client/ui.o -lncurses -lpthread -o bin/client

client.o: client/client.c defines.h
	gcc -c client/client.c -o client/client.o

ui.o: client/ui.c defines.h
	gcc -c client/ui.c -o client/ui.o

array_fnc.o: server/array_fnc.c
	gcc -c server/array_fnc.c -o server/array_fnc.o

clean:
	rm -rf bin */*.o *.pipe

folders:
	mkdir -p bin

debug: debug_server debug_client

debug_server: 
	gcc -g server/server.c server/array_fnc.c -o bin/server

debug_client:
	gcc -g client/client.c -lncurses -lpthread -o bin/client
