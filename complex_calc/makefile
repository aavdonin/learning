all: ccalc

ccalc: main.o lib.o liboper.so
	gcc main.o lib.o -L. -loper -Wl,-rpath,. -o ccalc

main.o: main.c
	gcc -c main.c

lib.o: lib.c
	gcc -c lib.c

liboper.so: add.o sub.o mul.o div.o
	gcc -shared add.o sub.o mul.o div.o -o liboper.so

add.o: add.c
	gcc -c -fPIC add.c

sub.o: sub.c
	gcc -c -fPIC sub.c

mul.o: mul.c
	gcc -c -fPIC mul.c

div.o: div.c
	gcc -c -fPIC div.c

clean:
	rm -rf *.o *.so *.a ccalc

