// Author: Emmett Kogan
// Last modified: 2/11/24

#include "FIFO.h"

// Initializes FIFO struct and relevant synchronization vars
int FIFO_init(FIFO_t *f, uint32_t depth, uint32_t width)
{
    f->depth = depth;
    f->width = width;

    if (pthread_mutex_init(&f->lock, NULL))
        goto mutex;

    if (sem_init(&f->count, 0, 0))
        goto sem;

    f->buffer = malloc(depth*width);
    if (!f->buffer)
        goto malloc;

    f->last = f->buffer + ((f->depth-1) * f->width);
    f->head = f->buffer;
    f->tail = f->buffer;

    return 0;

malloc:
    sem_destroy(&f->count);
sem:
    pthread_mutex_destroy(&f->lock);
mutex:
    return -1;
}

int FIFO_push(FIFO_t *f, void *data)
{
    int count;

//     // Pick up write lock and check if there is space to push to
//     if (pthread_mutex_lock(&f->lock))
//         goto lock;

    if (sem_getvalue(&f->count, &count))
        goto sem_getval;

    // If full release lock, on success return 1, otherwise return -1
    if (count >= f->depth)
        return pthread_mutex_unlock(&f->lock) ? -1 : 1;

    // Update FIFO
    memcpy(f->tail, data, f->width);
    f->tail = (f->tail == f->last) ? f->buffer : f->tail+f->width;
    if (sem_post(&f->count))
        goto sem_post;

    // Release write lock
//     if (pthread_mutex_unlock(&f->lock))
//         goto lock;

    return 0;


sem_post:
sem_getval:
    pthread_mutex_unlock(&f->lock);
lock:
    return -1;
}

// Simply does FIFO_push until there was a success or error
int FIFO_push_force(FIFO_t *f, void *data) {
    int ret;
    do {
        ret = FIFO_push(f, data);
        if (ret < 0)
            return -1;
    } while (ret > 0);
    return 0;
}

int FIFO_pop(FIFO_t *f, void *data)
{
    // Pick up read lock
    if (pthread_mutex_lock(&f->lock))
        goto lock;

    // Wait for data to read
    if (sem_wait(&f->count))
        goto sem_wait;

    // ** if blocked here

    // Copy data from FIFO to data and advance head pointer
    memcpy(data, f->head, f->width);
    f->head = (f->head == f->last) ? f->buffer : f->head + f->width;

    // Release read lock
    if (pthread_mutex_unlock(&f->lock))
        goto lock;

    return 0;

sem_wait:
    pthread_mutex_unlock(&f->lock);
lock:
    return -1;
}

// Resets FIFO struct essentially to state before init was called

// What if this is called when the FIFO wasn't initialized, it would try to free(null)
// So how can I tell if one of these is initialized... I could also just say that the behabior is undefined
int FIFO_clean(FIFO_t *f)
{
    f->depth = 0;
    f->width = 0;

    if (f->buffer) free(f->buffer);

    f->buffer = NULL;
    f->last = NULL;
    f->head = NULL;
    f->tail = NULL;

    // This way all cleanup is attempted no matter what
    int e = pthread_mutex_destroy(&f->lock);
    return (sem_destroy(&f->count) || e) ? -1 : 0;
}


