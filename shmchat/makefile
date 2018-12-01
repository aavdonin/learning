all: shmchat

shmchat: shmchat.o ui.o
	gcc shmchat.o ui.o -lncurses -lpthread -o shmchat

shmchat.o: shmchat.c ui.h defines.h
	gcc -c shmchat.c

ui.o: ui.c defines.h
	gcc -c ui.c

clean:
	rm -rf shmchat *.o

