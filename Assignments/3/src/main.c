#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#include "lib/FIFO.h"

#define DEPTH 5
#define MAX_STR_LEN 128

static void  *worker(void *args);

typedef struct node_t {
    int id, balance;
    sem_t s;
    struct node_t *next;
} node_t;

static node_t *head = NULL;

int main(int argc, char **argv) 
{
    FILE *fptr = fopen(argv[1], "r");
    char buffer[MAX_STR_LEN];

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
        sem_init(&new_node->s, 0, 1);
        new_node->next = NULL;

        if (head) {
            node_t *n = head;
            while (n->next)
                n = n->next;
            n->next = new_node;

        } else {
            head = new_node;
        }

        accounts++;
    }

    // Init threads
    int num_threads = atoi(argv[2]);
    pthread_t *threads = malloc(num_threads*sizeof(pthread_t));
    FIFO_t *fifos = malloc(num_threads*sizeof(FIFO_t));

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

    free(threads);
    free(fifos);
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
        // Get transfer from main thread
        FIFO_pop((FIFO_t *) args, buffer);

        // Terminal condition of thread
        if (buffer[0] == '\n')
            break;

        // Parse transfer arguements and convert to something useful
        char *_src, *_dest, *_amount, *tmp = buffer+9;
        _src = strtok_r(buffer+9, " ", &tmp);
        _dest = strtok_r(0, " ", &tmp);
        _amount = strtok_r(0, " ", &tmp);
        int src = atoi(_src), dest = atoi(_dest), amount = atoi(_amount);
        
        // If the same, transaction would do nothing anyways
        if (src == dest)
            continue;

        // Get pointers to both of the account nodes
        node_t *src_node = NULL, *dest_node = NULL, *iter = head;
        while(iter) {
            if (iter->id == src)
                src_node = iter;
            if (iter->id == dest)
                dest_node = iter;

            if (src_node && dest_node)
                break;
            iter = iter->next;
        }

        // Obtain the smaller account first
        if (src < dest) {
            sem_wait(&src_node->s);
            sem_wait(&dest_node->s);
        } else {
            sem_wait(&dest_node->s);
            sem_wait(&src_node->s);
        }

        // Do transfer
        src_node->balance  -= amount;
        dest_node->balance += amount;

        // Release accounts
        sem_post(&src_node->s);
        sem_post(&dest_node->s);
    }

    pthread_exit(NULL);
}
