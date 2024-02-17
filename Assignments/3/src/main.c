#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "lib/FIFO.h"

// Note depth isn't specified so I'm just defining this, could be depth of whatever tbh
#define DEPTH 5
#define MAX_STR_LEN 128

static void  *worker(void *args);

typedef struct node_t {
    int id, balance;
    struct node_t *next;
} node_t;

static node_t *head = NULL;
static pthread_mutex_t action_lock;
static pthread_mutex_t *account_locks;

int main(int argc, char **argv) 
{
    FILE *fptr = fopen(argv[1], "r");
    char buffer[MAX_STR_LEN]; // lines could be really big, need to ask for some constraint from Yavuz

    // Initialize account linked list
    int accounts = 0;
    while (1) {
        // Get line of input
        if (!fgets(buffer, MAX_STR_LEN, fptr) || buffer[0] == 'T')
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
        accounts++;
    }

    // Init threads and synchronization variables
    int num_threads = atoi(argv[2]);
    pthread_t *threads = malloc(num_threads*sizeof(pthread_t));
    FIFO_t *fifos = malloc(num_threads*sizeof(FIFO_t));

    account_locks = malloc(accounts*sizeof(pthread_mutex_t));
    pthread_mutex_init(&action_lock, NULL);
    for (int i = 0; i < accounts; i++)
        pthread_mutex_init(&account_locks[i], NULL);

    for (int i = 0; i < num_threads; i++) {
        FIFO_init(&fifos[i], DEPTH, MAX_STR_LEN);
        pthread_create(&threads[i], NULL, worker, (void *) &fifos[i]);
    }

    // Send transfers to threads in a round robin manner
    int idx = 0;
    while(buffer[0] == 'T') {
        // Send string to a worker thread
        FIFO_push(&fifos[idx], (void *) buffer);

        // Increment idx
        idx = (idx + 1) % num_threads; 

        // Reset buffer and get next line
        memset(buffer, 0, 128);
        fgets(buffer, 128, fptr);
    }

    // Cleanup
    void *res;
    buffer[0] = '\n';
    for (int i = 0; i < num_threads; i++) {
        FIFO_push(&fifos[i], buffer);
        pthread_join(threads[i], &res);
        FIFO_clean(&fifos[i]);
    }

    fclose(fptr);

    // Print accounts while freeing linked list
    node_t *tmp, *node = head;
    while (node) {
        printf("%d %d\n", node->id, node->balance);
        tmp = node;
        node = node->next;
        free(tmp);
    }

    return 0;
}

static void *worker(void *args) {
    char buffer[MAX_STR_LEN];
    while (1) {
        FIFO_pop((FIFO_t *) args, buffer);

        if (buffer[0] == '\n')
            break;

        // Parse transfer arguements
        char *_from, *_to, *_amount, *tmp = buffer+9;
        _from = strtok_r(buffer+9, " ", &tmp);
        _to = strtok_r(0, " ", &tmp);
        _amount = strtok_r(0, " ", &tmp);

        int from = atoi(_from), to = atoi(_to), amount = atoi(_amount);
        
        // Pick up action lock
        // If both account locks are available, take them and release action lock
            // Otherwise release all locks and try again after a sleep or something
        
        // Do transfer

        // Pick up action lock
        // Once obtained release both account locks


        // Reset buffer for next transfer
        memset(buffer, 0, MAX_STR_LEN);
    }

    printf("Thread exiting!\n");
    pthread_exit(0);
}
