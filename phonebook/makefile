all: phonebook

phonebook: main.o lib.o
	gcc main.o lib.o -o phonebook

main.o: main.c
	gcc -c main.c

lib.o: lib.c
	gcc -c lib.c

clean:
	rm -rf *.o phonebook
