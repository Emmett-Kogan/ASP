// Author: Emmett Kogan
// Last modified: 2/7/24

#ifndef FIFO_H
#define FIFO_H

#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct FIFO_t {
    uint32_t depth, width, count;
    pthread_mutex_t lock;
    pthread_cond_t full, empty;
    void *buffer;
    void *last, *head, *tail;
} FIFO_t;

int FIFO_init(FIFO_t *f, uint32_t depth, uint32_t width);
int FIFO_push(FIFO_t *f, void *data);
int FIFO_pop(FIFO_t *f, void *data);
int FIFO_clean(FIFO_t *f);

#endif
