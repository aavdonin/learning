#include <arpa/inet.h>

unsigned short int checksum(unsigned short int *addr, unsigned int count) {
        unsigned long int sum = 0;
        while(count > 1) {
                sum += *addr++;
                count -= 2;
        }
        if(count > 0) {
                sum += ((*addr) & htons(0xFF00));
        }
        while (sum >> 16) {
                sum = (sum & 0xFFFF) + (sum >> 16);
        }
        sum = ~sum;
        return ((unsigned short int) sum);
}
