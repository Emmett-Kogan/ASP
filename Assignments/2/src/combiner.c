#include <pthread.h>
#include <stdio.h>

#include "lib/wrline.h"
#include "lib/FIFO.h"
#include "lib/threads.h"
#define MAX_STR_LEN 32

int main(int argc, char **argv)
{
    // Get and validate args
    if (argc < 3)
        goto args;

    int depth = atoi(argv[1]), num_threads = atoi(argv[2]);
    if (!depth)
        goto depth;

    if (!num_threads)
        goto num_threads;

    // Setup FIFOs and pthread structs
    int idx;
    void *res;
    pthread_t *threads = (pthread_t *) malloc(num_threads*sizeof(pthread_t));
    if (!threads)
        goto threads_malloc;

    FIFO_t *fifos = (FIFO_t *) malloc(num_threads*sizeof(FIFO_t));
    if (!fifos)
        goto fifos_malloc;

    for (idx = 0; idx < num_threads; idx++) {
        int tmp = FIFO_init(&fifos[idx], depth, MAX_STR_LEN);
        tmp |= pthread_create(&threads[idx], NULL, reducer, &fifos[idx]);
        if (tmp)
            goto init_fail;
    }

    // Modified version of mapper
    idx = 0;
	char inbuff[MAX_STR_LEN], outbuff[MAX_STR_LEN];
    int *fifo_map = (int *) malloc(num_threads*sizeof(int));
    memset(fifo_map, -1, num_threads*sizeof(int));

	while(1) {
		memset(inbuff, 0, MAX_STR_LEN);

		if(!readline(inbuff, MAX_STR_LEN) || !inbuff[0] || inbuff[0] == '\n') {
            // send a newline char to each buffer to signal no more data
            outbuff[0] = '\n';
            for (int i = 0; i < num_threads; i++)
                if (FIFO_push_force(&fifos[i], outbuff))
                    goto general_fail;
            break;
        }

        // Get ID
        char tmp[4];
        memcpy(tmp, inbuff+1, 4);
        int id = atoi(tmp);

        // Copy the ID and action to the new char array
        memcpy(outbuff, inbuff, 6);
        for (idx = 8; idx < MAX_STR_LEN && inbuff[idx] != ')'; idx++)
            outbuff[idx] = inbuff[idx];

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

		// Get mapping of which fifo the string needs to go to
		int i;
		for (i = 0; i < num_threads; i++)
            if (id == fifo_map[i])
                break;

        // If no mapping, create one
        if (i == num_threads) {
            // Iterate to find first available fifo and update the fifo_map, and use that index
            i = 0;
            while (fifo_map[i] < 0) i++;

            // Not enough Q's to provide a unique map, not certain if this is reachable or not
            if (i == num_threads)
                goto general_fail;
            fifo_map[i] = id;
        }

        // Send string the mapped fifo
        if (FIFO_push_force(&fifos[i], outbuff))
            goto general_fail;
	}

    // Proper exit
    idx = 0;
    while (num_threads > 0) {
        pthread_join(threads[idx++], &res);
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

general_fail:
    idx = num_threads;
init_fail:
    for (int i = 0; i < idx; i++) {
        pthread_cancel(threads[i]);
        pthread_join(threads[i], &res);
        FIFO_clean(&fifos[i]);
    }
    free(fifos);
fifos_malloc:
    free(threads);
threads_malloc:
    return -1;
}

