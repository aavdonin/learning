all: raw_client_udp

raw_client_udp: raw_client_udp.o csum.o recv_raw.o
	gcc raw_client_udp.o csum.o recv_raw.o -o ../bin/raw_client_udp

raw_client_udp.o: raw_client_udp.c ../defines.h
	gcc -c raw_client_udp.c

csum.o: csum.c
	gcc -c csum.c

recv_raw.o: recv_raw.c ../defines.h
	gcc -c recv_raw.c

sniff_udp: sniff_udp.c
	gcc sniff_udp.c -o ../bin/sniff_udp

clean:
	rm *.o

