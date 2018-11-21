#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "defines.h"

void *loader_job(void *args) {
    printf("Loader woke up\n");
    while (1) {
        int chosen_store = rand()%STORES_CNT;
        int mst; //mutex status
        if ((mst = pthread_mutex_trylock(&mutex[chosen_store])) == EBUSY) {
            printf("Loader wanted to put some goods in Store %d, " \
            "but it's occupied\n", chosen_store+1);
            continue;
        }
        else if (mst != 0) {
            fprintf(stderr,"Loader mutex error\n");
            exit(1);
        }
        int load = 300 + rand()%200;
        store[chosen_store] += load;
        printf("Loader put %d goods into Store %d.\n", load, chosen_store+1);
        printf("Store %d now has %d goods\n", \
        chosen_store+1, store[chosen_store]);
        sleep(1);
        pthread_mutex_unlock(&mutex[chosen_store]);
    }
}
