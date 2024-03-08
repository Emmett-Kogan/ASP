#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include "lib/FIFO.h"
#include "lib/threads.h"
#include "lib/wrline.h"

#define MAX_STR_LEN 32

int main(int argc, char **argv) {
    // Process command line args
    int depth = atoi(argv[1]), num_threads = atoi(argv[2]);
    
    // Init FIFOs
    FIFO_t *fifos = (FIFO_t *) mmap(NULL, sizeof(FIFO_t)*num_threads, PROT_READ | PROT_WRITE, 
                                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    for (int i = 0; i < num_threads; i++)
        FIFO_init(&fifos[i], depth, MAX_STR_LEN);

    // Spawn children
    for (int i = 0; i < num_threads; i++) {
        if (!fork()) {
            // Child, I think this should be fine lol
            reducer((void *) &fifos[i]);
            exit(0);
        }
    }

    // Modified version of mapper
    char inbuff[MAX_STR_LEN], outbuff[MAX_STR_LEN];
    int *fifo_map = (int *) malloc(num_threads*sizeof(int));
    memset(fifo_map, -1, num_threads*sizeof(int));

    while(1) {
        memset(inbuff, 0, MAX_STR_LEN);
        memset(outbuff, 0, MAX_STR_LEN);

        if(!readline(inbuff, MAX_STR_LEN) || !inbuff[0] || inbuff[0] == '\n') {
            // send a newline char to each buffer to signal no more data
            outbuff[0] = '\n';
            for (int i = 0; i < num_threads; i++)
                FIFO_push(&fifos[i], outbuff, 0);
            break;
        }

        // Get ID
        char tmp[4];
        memcpy(tmp, inbuff+1, 4);
        int id = atoi(tmp);

        // Build intermediate string to send to thread
        int idx = 6;
        memcpy(outbuff, inbuff, 6);
        for (int i = 8; i < MAX_STR_LEN && inbuff[i] != ')'; i++)
            outbuff[idx++] = inbuff[i];

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
            while (fifo_map[i] > 0) i++;

            // Not enough Q's to provide a unique map, not certain if this is reachable or not
            if (i == num_threads) {
                printf("Error: More users than threads\n");
                return -1;
            }
            fifo_map[i] = id;
        }

        // Send string the mapped fifo
        FIFO_push(&fifos[i], outbuff, 0);
	}

    // Wait for all children to die
    while(wait(NULL) > 0);

    // Cleanup
    for (int i = 0; i < num_threads; i++)
        FIFO_clean(&fifos[i]);
    munmap(fifos, num_threads*sizeof(FIFO_t));
    free(fifo_map);
    
    return 0;
}