all: main.o defines.h
	gcc main.o -lncurses -o fileman

main.o: main.c defines.h
	gcc -c main.c

debug: main.c defines.h
	gcc -g main.c -lncurses -o fileman

clean:
	rm -rf *.o fileman

