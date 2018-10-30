all: main.o file.o
	gcc main.o file.o -lncurses -o editor

main.o: main.c
	gcc -c main.c

file.o: file.c
	gcc -c file.c

clean:
	rm -rf *.o *.txt editor

