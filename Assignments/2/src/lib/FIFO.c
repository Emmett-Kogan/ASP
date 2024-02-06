// Author: Emmett Kogan
// Last modified: 2/4/24

#include "FIFO.h"

// Initializes FIFO struct and relevant synchronization vars
int FIFO_init(FIFO_t *f, uint32_t depth, uint32_t width) {
    f->depth = depth;
    f->width = width;

    if (pthread_mutex_init(&f->rlock, NULL)) return -1;
    if (pthread_mutex_init(&f->wlock, NULL)) return -1;
    if (sem_init(&f->count, 0, 0)) return -1;

    f->buffer = malloc(depth*width);
    if (!f->buffer) return -1;

    f->last = f->buffer + ((f->depth-1) * f->width);
    f->head = f->buffer;
    f->tail = f->buffer;

    return 0;
}

/*
 * Hypothetically if I wanted to do this with a condition variable, I could do one like in the examples
 * execpt instead of being directly related to count, like with the counting semaphore approach, it would
 * be related indirectly via a full/empty flag I guess that would be the condition, and a mutex. If I want
 * both writes and reads to be done one at a time to prevent race conditions, there would need to be 3 mutexes
 * total this way, one for read, one for write, and one for full/empty. So, if a thread picks up for instance the write lock,
 * and then tries to obtain the full/empty lock and gets blocked, then it would have to wait on whatever is holding it, 
 * which could not be a write I think, it would have to be a read because of the write lock. Since it would be used essentially
 * as not empty in the read case, then idk if I would really need 3 locks, since it would be impossible for all but one thread to work on the FIFO at any time.
 * This may be good? But, the way I have it set up rn, I can have a read and write happening concurrently I think, which may be better, but seems slightly more expensive.
 */

int FIFO_push(FIFO_t *f, void *data) {
    int count;

    // Pick up write lock and check if there is space to push to
    pthread_mutex_lock(&f->wlock);
    if(sem_getvalue(&f->count, &count) || count >= f->depth) {
        pthread_mutex_unlock(&f->wlock);
        return -1;
    }

    // Update FIFO
    memcpy(f->tail, data, f->width);
    f->tail = (f->tail == f->last) ? f->buffer : f->tail+f->width;
    sem_post(&f->count);

    // Release write lock
    pthread_mutex_unlock(&f->wlock);
    return 0;
}

int FIFO_pop(FIFO_t *f, void *data) {
    // Pick up read lock
    pthread_mutex_lock(&f->rlock);

    // Wait for data to read
    sem_wait(&f->count);

    // Copy data from FIFO to data and advance head pointer
    memcpy(data, f->head, f->width);
    f->head = (f->head == f->last) ? f->buffer : f->head + f->width;

    // Release read lock
    pthread_mutex_unlock(&f->rlock);
    return 0;
}

// Resets FIFO struct essentially to state before init was called
int FIFO_clean(FIFO_t *f) {
    f->depth = 0;
    f->width = 0;

    if (pthread_mutex_destroy(&f->rlock)) return -1;
    if (pthread_mutex_destroy(&f->wlock)) return -1;
    if (sem_destroy(&f->count)) return -1;
    free(f->buffer);

    f->buffer = NULL;
    f->last = NULL;
    f->head = NULL;
    f->tail = NULL;

    return 0;
}


