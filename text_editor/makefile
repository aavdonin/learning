all: main.o file.o text.o
	gcc main.o file.o text.o -lncurses -o editor

main.o: main.c
	gcc -c main.c

file.o: file.c
	gcc -c file.c

text.o: text.c
	gcc -c text.c

debug: main.c file.c text.c
	gcc -g main.c file.c text.c -lncurses -o editor

clean:
	rm -rf *.o *.txt editor

