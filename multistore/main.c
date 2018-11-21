#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "defines.h"
#include "loader.h"
#include "buyer.h"

buyer buyers[BUYERS_CNT];
unsigned int store[STORES_CNT];
pthread_mutex_t mutex[STORES_CNT];

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
