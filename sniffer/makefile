all: sniffer

sniffer: sniff.o print.o
	gcc sniff.o print.o -lpcap -o sniff

sniff.o: sniff.c defines.h
	gcc -c sniff.c

print.o: print.c defines.h
	gcc -c print.c

