#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#define STORES_CNT 5
#define BUYERS_CNT 3

typedef struct {
    char id;
    unsigned int need;
} buyer;

buyer buyers[BUYERS_CNT];
unsigned int store[STORES_CNT];
static pthread_mutex_t mutex[STORES_CNT];

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

void *buyer_job(void *args) {
    buyer *me = (buyer*) args; //who am i?
    int mst; //mutex status
    while (me->need > 0) {
        int chosen_store = rand()%STORES_CNT;
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
        sleep(1);
        pthread_mutex_unlock(&mutex[chosen_store]);
        sleep(3);
    }
}

int main() {
    int i;
    srand((unsigned) time(NULL));
    for (i=0; i<STORES_CNT; i++) { //store initialization
        pthread_mutex_init(&mutex[i], NULL);
        //mutex[i] = PTHREAD_MUTEX_INITIALIZER;
        store[i] = 1000 + rand()%200;
        printf("Store %d initialized to %d goods\n", i+1, store[i]);
    }
    for (i=0; i<BUYERS_CNT; i++) { //initialization of buyers needs
        buyers[i].id = i+1;
        buyers[i].need = 3000 + rand()%500;
        printf("Buyer %d wants to buy %d goods\n",buyers[i].id, buyers[i].need);
    }
    printf("\nOkay, let's go...\n\n");
    
    pthread_t t_loader, t_buyer[BUYERS_CNT];
    int status, *result;
    status = pthread_create(&t_loader, NULL, loader_job, NULL);//start loader
    if (status != 0) {
        fprintf(stderr,"Unable to create 'loader' thread!\n");
        exit(1);
    }
    for (i=0; i<BUYERS_CNT; i++) { //start buyers
        status = pthread_create(&t_buyer[i], NULL, buyer_job, &buyers[i]);
        if (status != 0) {
            fprintf(stderr,"Unable to create 'buyer %d' thread!\n", i+1);
            exit(1);
        }
    }
    for (i=0; i<BUYERS_CNT; i++) { //wait for buyers to finish shopping
        status = pthread_join(t_buyer[i], NULL);
        if (status != 0) {
            fprintf(stderr,"Join 'buyer %d' thread error\n", i+1);
            exit(1);
        }
    }
    printf("All done...\n");
}
