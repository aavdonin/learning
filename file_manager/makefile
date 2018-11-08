all: main.o
	gcc main.o -lncurses -o fileman

main.o: main.c
	gcc -c main.c

debug: main.c
	gcc -g main.c -lncurses -o fileman

clean:
	rm -rf *.o fileman

