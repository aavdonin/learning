all: main.o loader.o buyer.o
	gcc main.o loader.o buyer.o -o store -lpthread

main.o: main.c defines.h loader.h buyer.h
	gcc -c main.c

loader.o: loader.c defines.h
	gcc -c loader.c

buyer.o: buyer.c defines.h
	gcc -c buyer.c

clean:
	rm *.o store