all: ccalc

ccalc: main.o lib.o libs
	gcc main.o lib.o -ldl -o ccalc

libs: plugin/libadd.so plugin/libsub.so plugin/libmul.so plugin/libdiv.so

main.o: main.c
	gcc -c main.c

lib.o: lib.c
	gcc -c lib.c

plugin/libadd.so: add.o
	mkdir -p plugin
	gcc -shared -nostartfiles add.o -o plugin/libadd.so

plugin/libsub.so: sub.o
	mkdir -p plugin
	gcc -shared -nostartfiles sub.o -o plugin/libsub.so

plugin/libmul.so: mul.o
	mkdir -p plugin
	gcc -shared -nostartfiles mul.o -o plugin/libmul.so

plugin/libdiv.so: div.o
	mkdir -p plugin
	gcc -shared -nostartfiles div.o -o plugin/libdiv.so

add.o: add.c
	gcc -c -fPIC add.c

sub.o: sub.c
	gcc -c -fPIC sub.c

mul.o: mul.c
	gcc -c -fPIC mul.c

div.o: div.c
	gcc -c -fPIC div.c

clean:
	rm -rf *.o *.so *.a ccalc plugin

