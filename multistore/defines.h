#ifndef _DEFINES_H_
#define _DEFINES_H_

#define STORES_CNT 5
#define BUYERS_CNT 3

typedef struct {
    char id;
    unsigned int need;
} buyer;

extern buyer buyers[BUYERS_CNT];
extern unsigned int store[STORES_CNT];
extern pthread_mutex_t mutex[STORES_CNT];

#endif