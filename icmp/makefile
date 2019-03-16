all: ping traceroute

dirs:
	mkdir -p bin

ping: dirs ping.o icmp_send.o csum.o
	gcc ping.o icmp_send.o csum.o -o bin/ping

ping.o: ping.c defines.h icmp_send.h
	gcc -c ping.c

icmp_send.o: icmp_send.c defines.h csum.h
	gcc -c icmp_send.c

csum.o: csum.c
	gcc -c csum.c

traceroute: dirs traceroute.o icmp_send.o csum.o
	gcc traceroute.o icmp_send.o csum.o -o bin/traceroute

traceroute.o: traceroute.c defines.h icmp_send.h
	gcc -c traceroute.c

clean:
	rm -rf bin *.o

