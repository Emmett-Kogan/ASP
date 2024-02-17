#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "lib/FIFO.h"
#include "lib/threads.h"

// Note depth isn't specified so I'm just defining this, could be depth of whatever tbh
#define DEPTH 5
#define MAX_STR_LEN 128

typedef struct node_t {
    int id, balance;
    struct node_t *next;
} node_t;

static node_t *head = NULL;

int main(int argc, char **argv) 
{
    FILE *fptr = fopen(argv[1], "r");
    char buffer[128]; // lines could be really big, need to ask for some constraint from Yavuz

    while (1) {
        // Get line of input
        if (!fgets(buffer, 128, fptr) || buffer[0] == 'T')
            break;
        
        // Parse line with strtok
        char *_id, *_balance, *tmp = buffer;
        _id = strtok_r(buffer, " ", &tmp);
        _balance = strtok_r(0, " ", &tmp);
        int id = atoi(_id), balance = atoi(_balance);

        // Push account into the linked list
        node_t *new_node = (node_t *) malloc(sizeof(node_t));
        new_node->id = id;
        new_node->balance = balance;
        new_node->next = head;
        head = new_node;

    }

    // Want to print a traversal of the LL here once I'm done
    printf("Traversal:\n");
    node_t *node = head;
    while(node) {
        printf("ID: %d\tBalance: %d\n", node->id, node->balance);
        node = node->next;
    }

    // Need to init threads here    
    int num_threads = atoi(argv[2]);

    pthread_t *threads = malloc(num_threads*sizeof(pthread_t));
    FIFO_t *fifos = malloc(num_threads*sizeof(FIFO_t));

    for (int i = 0; i < num_threads; i++) {
        FIFO_init(&fifos[i], DEPTH, MAX_STR_LEN);
        pthread_create(&threads[i], NULL, worker, (void *) &fifos[i]);
    }

    // Step 2 is sending the tranfer strings to worker threads and having worker threads do something with them
        // Want to do a do while since buffer already holds the first command
        // Or maybe just a while where the condition is buffer[0] == 'T'

    printf("\nTransfers:\n");
    while(buffer[0] == 'T') {
        printf("%s", buffer);
        memset(buffer, 0, 128);
        fgets(buffer, 128, fptr);

        // Want to send each string to the worker thread for it to deal with

    }

    // Step 3 is getting the worker threads to actually do their job



    // Cleanup
    void *res;
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], &res);
        FIFO_clean(&fifos[i]);
    }

    fclose(fptr);
    return 0;
}