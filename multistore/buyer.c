#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "defines.h"

void *buyer_job(void *args) {
    buyer *me = (buyer*) args; //who am i?
    int mst; //mutex status
    while (me->need > 0) {
        int chosen_store = rand()%5;
        if ((mst = pthread_mutex_trylock(&mutex[chosen_store])) == EBUSY) {
            printf("Buyer %d went to Store %d, but it's occupied\n", \
            me->id, chosen_store+1);
            continue;
        }
        else if (mst != 0) {
            fprintf(stderr,"Buyer %d mutex error\n", me->id);
            exit(1);
        }
        if (store[chosen_store] <= me->need) { //buy everything and leave
            me->need -= store[chosen_store];
            printf("Buyer %d bought %d goods from Store %d\n", \
            me->id, store[chosen_store], chosen_store+1);
            store[chosen_store] = 0;
        }
        else {  //buy a few
            store[chosen_store] -= me->need;
            printf("Buyer %d bought his last %d goods at Store %d." \
            " %d goods left in the Store\n", me->id, me->need, chosen_store+1, \
            store[chosen_store]);
            me->need = 0;
            pthread_mutex_unlock(&mutex[chosen_store]);
            break;
        }
        pthread_mutex_unlock(&mutex[chosen_store]);
        sleep(3);
    }
}
