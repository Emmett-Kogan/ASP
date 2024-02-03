#ifndef FIFO_H
#define FIFO_H

#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
    In the case of assignment 2, I need a FIFO struct that I can make variably
    long, so probably using the head since I'm getting the info of how long at
    compile time,

    The actual array/buffer whatever will be hidden from the user since there
    will be FIFO_push and FIFO_pop as pretty much the only way of interfacing
    with it

    Might want to have an enum for return values here depending on how complex
    those become
 */

// Note that the void pointers passed to push and pop should point to buffers that are at least as large as entry_size

// Head should point to the first valid unless it is empty, where it will point to tail
// tail should point to the

typedef struct FIFO_t {
    uint32_t depth, width;
    pthread_mutex_t lock;
    sem_t count;
    void *buffer;
    void *last, *head, *tail;
} FIFO_t;

int FIFO_init(FIFO_t *f, uint32_t depth, uint32_t width);
int FIFO_push(FIFO_t *f, void *data);
int FIFO_pop(FIFO_t *f, void *data);

#endif
