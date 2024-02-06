#include <pthread.h>
#include <stdio.h>

#include "lib/FIFO.h"
#define MAX_STR_LEN 32

// These will basically just be the programs from 1 but modified to read
// from/write to the FIFO struct
static void *reducer(void *args);

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
    for (idx = 0; idx < num_threads; idx++) {
        int tmp = FIFO_init(&fifos[idx], depth, MAX_STR_LEN);
        tmp |= pthread_create(&threads[idx], NULL, consumer, &fifos[idx]);
        if (tmp)
            goto init_fail;
    }

    // Parse data like mapper and send it to the respective FIFOs
    // Once the end has been reached, signal the threads that no more
    // input will be sent by sending just a newline character

    // Might also have to build a mapping of which fifo is associated with which ID as I go
    // So having an array that holds each ID and have the ID be at the same index as the index of the FIFO in fifos would be good

    idx = 0;
	char inbuff[MAX_STR_LEN], outbuff[MAX_STR_LEN];
    int *fifo_map = (int *) malloc(num_threads*sizeof(int));
    memset(fifo_map, -1, num_threads*sizeof(int));

	while(1) {
		memset(inbuff, 0, MAX_STR_LEN);

		if(!readline(inbuff, MAX_STR_LEN) || !inbuff[0] || inbuff[0] == '\n')
            break;

        // Get ID
        char tmp[4];
        memcpy(tmp, inbuff+1, 4);
        int id = atoi(tmp);

        // Copy the ID and action to the new char array
        memcpy(outbuff, inbuff, 6);
        for (idx = 8; idx < MAX_STR_LEN && inbuff[idx] != ')'; idx++)
            outbuff[idx] = inbuff[idx];
        // Now index should point to the byte after the end of the chars

        // Append the value to the end
		switch (inbuff[6]) {
		case 'P':
            memcpy(outbuff+idx, ",50)\n", 5);
			break;
		case 'L':
            memcpy(outbuff+idx, ",20)\n", 5);
			break;
		case 'D':
            memcpy(outbuff+idx, ",-10)\n", 6);
			break;
		case 'C':
            memcpy(outbuff+idx, ",30)\n", 5);
			break;
		case 'S':
            memcpy(outbuff+idx, ",40)\n", 5);
			break;
		default:
            printf("Invalid action");
            continue;
		}

		// Send to corresponding FIFO and make mapping if necessary
		// To check which FIFO, iterate over FIFO map to get a matching index
		int i;
		for (i = 0; i < num_threads; i++)
            if (id == fifo_map[i])
                break;

        // If no match
        if (i == num_threads) {
            // Iterate to find first available fifo and update the fifo_map, and use that index

        }

        // Send string to that index
	}




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

static void *reducer()
{
    return NULL;
}
