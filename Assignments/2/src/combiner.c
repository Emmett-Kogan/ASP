#include <pthread.h>
#include <stdio.h>

#include "lib/FIFO.h"
#define MAX_STR_LEN 32

// These will basically just be the programs from 1 but modified to read
// from/write to the FIFO struct
static void *consumer(void *args);

int main(int argc, char **argv)
{
    // argv[1] -> depth of FIFOs, argv[2] -> number of FIFOs/unique IDs I guess
    if (argc < 3)
        goto args;

    int depth = atoi(argv[1]), num_threads = atoi(argv[2]), idx;

    if (!depth)
        goto depth;

    if (!num_threads)
        goto num_threads;

    pthread_t *threads = (pthread_t *) malloc(num_threads*sizeof(pthread_t));
    if (!threads)
        goto threads_malloc;

    FIFO_t *fifos = (FIFO_t *) malloc(num_threads*sizeof(FIFO_t));
    if (!fifos)
        goto fifos_malloc;

    // If this fails the program just can't run
    int tmp = 0;
    for (idx = 0; idx < num_threads; idx++) {
        tmp |= FIFO_init(&fifos[idx], depth, MAX_STR_LEN);
        tmp |= pthread_create(&threads[idx], NULL, consumer, &fifos[idx]);
        if (tmp)
            goto init_fail;
    }

    // Parse data like mapper and send it to the respective FIFOs
    // Once the end has been reached, signal the threads that no more
    // input will be sent by sending just a newline character

    // Might also have to build a mapping of which fifo is associated with which ID as I go
    // So having an array that holds each ID and have the ID be at the same index as the index of the FIFO in fifos would be good

    // Wait for each thread to join before returning
    idx = 0;
    int res;
    while (num_threads > 0) {
        pthread_join(&threads[idx++], &res);
        FIFO_clean(&fifos[idx]);
    }

    free(threads);
    free(fifos);
    return 0;



args:
    printf("Expecting depth and number of threads as arguements\n");
    return -1;
depth:
    printf("Depth of FIFOs must be at least one\n");
    return -1;
num_threads:
    printf("Number of threads must be at least one\n");
    return -1;

init_fail:
    // Kill all created threads, and clean their FIFO
    for (int i = 0; i < idx; i++) {
        pthread_kill(&threads[i]);
        FIFO_clean(&fifos[i]);
    }

    // Free all dynamically allocated memory
    free(fifos);
fifos_malloc:
    free(threads);
threads_malloc:
    return -1;
}

static void *mapper()
{
    return NULL;
}

static void *consumer()
{
    return NULL;
}
