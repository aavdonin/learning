all: main.o fs.o ui.o
	gcc main.o fs.o ui.o -lncurses -o fileman

main.o: main.c fs.h ui.h
	gcc -c main.c

fs.o: fs.c fs.h ui.h
	gcc -c fs.c

ui.o: ui.c fs.h
	gcc -c ui.c

debug: main.c fs.c fs.h ui.c ui.h
	gcc -g main.c fs.c ui.c -lncurses -o fileman

clean:
	rm -rf *.o fileman

