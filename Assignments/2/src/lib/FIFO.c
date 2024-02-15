// Author: Emmett Kogan
// Last modified: 2/11/24

#include "FIFO.h"

// Initializes FIFO struct and relevant synchronization vars
int FIFO_init(FIFO_t *f, uint32_t depth, uint32_t width)
{
    f->depth = depth;
    f->width = width;
    f->count = 0;

    if (pthread_mutex_init(&f->lock, NULL))
        goto mutex;

    if (pthread_cond_init(&f->full, NULL))
        goto full;
    if (pthread_cond_init(&f->empty, NULL))
        goto empty;

    f->buffer = malloc(depth*width);
    if (!f->buffer)
        goto malloc;

    f->last = f->buffer + ((f->depth-1) * f->width);
    f->head = f->buffer;
    f->tail = f->buffer;

    return 0;

empty:
    pthread_cond_destroy(&f->full);
full:
malloc:
    pthread_mutex_destroy(&f->lock);
mutex:
    return -1;
}

int FIFO_push(FIFO_t *f, void *data)
{
    // Pick up lock
    if(pthread_mutex_lock(&f->lock))
        goto mutex;

    // If full wait on cond var
    if (f->count == f->depth)
        if (pthread_cond_wait(&f->full, &f->lock))
            goto cond_wait;

    // Update FIFO
    memcpy(f->tail, data, f->width);
    f->tail = (f->tail == f->last) ? f->buffer : f->tail+f->width;
    f->count++;

    // Release lock
    if (pthread_mutex_unlock(&f->lock))
        goto mutex;

    // Signal empty (this either does nothing or unblocks a consumer)
    return pthread_cond_signal(&f->empty);

cond_wait:
    pthread_mutex_unlock(&f->lock);
mutex:
    return -1;
}

int FIFO_pop(FIFO_t *f, void *data)
{
    // Pick up lock
    if (pthread_mutex_lock(&f->lock))
        goto mutex;

    // If empty wait on cond var
    if (f->count == 0)
        if (pthread_cond_wait(&f->empty, &f->lock))
            goto cond_wait;

    // Read data and update data structure
    memcpy(data, f->head, f->width);
    f->head = (f->head == f->last) ? f->buffer : f->head + f->width;
    f->count--;

    // Release lock
    if (pthread_mutex_unlock(&f->lock))
        goto mutex;

    // Signal full (either does nothing or unblocks a producer)
    return pthread_cond_signal(&f->full);

cond_wait:
    pthread_mutex_unlock(&f->lock);
mutex:
    return -1;
}

// Resets FIFO struct essentially to state before init was called
int FIFO_clean(FIFO_t *f)
{
    f->depth = 0;
    f->width = 0;

    if (f->buffer) free(f->buffer);

    f->buffer = NULL;
    f->last = NULL;
    f->head = NULL;
    f->tail = NULL;
    f->count = 0;

    // This way all cleanup is attempted no matter what
    pthread_cond_destroy(&f->full);
    pthread_cond_destroy(&f->empty);
    return pthread_mutex_destroy(&f->lock);
}

