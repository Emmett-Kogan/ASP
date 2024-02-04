// Author: Emmett Kogan
// Last modified: 2/4/24

#include "FIFO.h"

// Just needs to initialize each of the FIFO's variables
// Note that this doesn't validate the depth and entry_size as far as the upper bounds, I know malloc will do something weird (by allocating the specified number of bytes which will be very small), and to fix this I could just use calloc, but...idk if I want to use calloc

int FIFO_init(FIFO_t *f, uint32_t depth, uint32_t width) {
    f->depth = depth;
    f->width = width;

    if (pthread_mutex_init(&f->lock, NULL))
        perror("Pthread mutex init in FIFO");

    if (sem_init(&f->count, 0, 0))
        perror("Semaphore init in FIFO");

    f->buffer = malloc(depth*width);
    if (!f->buffer)
        perror("Malloc in FIFO");

    f->last = f->buffer + ((f->depth-1) * f->width);
    f->head = f->buffer;
    f->tail = f->buffer;

    return 0;
}

// While pushing, should simply compare depth to value of semaphore, if they are
// the same, then it should wait until the semaphore is less than the max value,
// then push to the FIFO and return
int FIFO_push(FIFO_t *f, void *data) {
    int count;

    // Check if there is space to push88
    if(sem_getvalue(&f->count, &count) || count >= f->depth) return -1;

    // Update FIFO
    memcpy(f->tail, data, f->width);
    f->tail = (f->tail == f->last) ? f->buffer : f->tail+f->width;
    sem_post(&f->count);

    return 0;
}

// When popping, should try to take the lock, if something else is reading it will block, otherwise we good. Once we have the lock we wait on the semaphore, and once we get past that we simply copy the data off the semaphore to the calling function's buffer
int FIFO_pop(FIFO_t *f, void *data) {
    // Pick up lock
    pthread_mutex_lock(&f->lock);

    // Wait on semaphore
    sem_wait(&f->count);

    // Copy data from FIFO to data and advance head pointer
    memcpy(data, f->head, f->width);
    f->head = (f->head == f->last) ? f->buffer : f->head + f->width;


    // Release lock
    pthread_mutex_unlock(&f->lock);
    return 0;
}
