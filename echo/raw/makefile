all: raw_client_udp

raw_client_udp: raw_client_udp.o
	gcc raw_client_udp.o -o ../bin/raw_client_udp

raw_client_udp.o: raw_client_udp.c ../defines.h
	gcc -c raw_client_udp.c

sniff_udp: sniff_udp.c
	gcc sniff_udp.c -o ../bin/sniff_udp

