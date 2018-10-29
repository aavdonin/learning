all: main.o
	gcc main.o -lncurses -o editor

main.o: main.c
	gcc -c main.c

clean:
	rm -rf *.o *.txt editor

