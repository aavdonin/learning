all: main.o parser.o
	gcc main.o parser.o -o myshell

main.o: main.c parser.h defines.h
	gcc -c main.c

parser.o: parser.c defines.h
	gcc -c parser.c

clean:
	rm *.o myshell

debug: main.c parser.c parser.h defines.h
	gcc -g main.c parser.c -o myshell