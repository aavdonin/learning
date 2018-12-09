#include <stdio.h>
#include <string.h>

#include "cli_tcp.h"
#include "cli_udp.h"
#include "../defines.h"

int main(int argc, char *argv[]) {
    char cmd[CMDSIZE];
    while (1) {
        printf("1) TCP\n2) UDP\n0) Exit\n>");
        fgets(cmd, CMDSIZE, stdin);
        if (strcmp(cmd, "1\n") == 0) {
            tcp_client();
        }
        else if (strcmp(cmd, "2\n") == 0) {
            udp_client();
        }
        else if (strcmp(cmd, "0\n") ==0) {
            break;
        }
        else {
            printf("Wrong command. Try again.\n");
        }
    }
    return 0;
}
