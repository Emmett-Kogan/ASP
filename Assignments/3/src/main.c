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
static pthread_mutex_t action_lock;

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

    // Init threads and synchronization variables
    int num_threads = atoi(argv[2]);
    pthread_t *threads = malloc(num_threads*sizeof(pthread_t));
    FIFO_t *fifos = malloc(num_threads*sizeof(FIFO_t));
  
    pthread_mutex_init(&action_lock, NULL);

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

        // Parse transfer arguements and convert to something useful
        char *_from, *_to, *_amount, *tmp = buffer+9;
        _from = strtok_r(buffer+9, " ", &tmp);
        _to = strtok_r(0, " ", &tmp);
        _amount = strtok_r(0, " ", &tmp);
        int from = atoi(_from), to = atoi(_to), amount = atoi(_amount);
        
        // Get pointers to both of the account nodes
        node_t *acc_from = head;
        while(acc_from) {
            if (acc_from->id == from)
                break;
            acc_from = acc_from->next;
        }

        node_t *acc_to = head;
        while(acc_to) {
            if (acc_to->id == to)
                break;
            acc_to = acc_to->next;
        }

        // This basically polls the two required semaphores until they are available
        int s_from = 0, s_to = 0;
        while (1) {
            // Get action lock
            pthread_mutex_lock(&action_lock);

            // Get values of both account semaphores
            sem_getvalue(&acc_from->s, &s_from);
            sem_getvalue(&acc_to->s, &s_to);

            // If both available proceed
            if (s_from == 1 && s_to == 1)
                break;
            
            // Otherwise give up the action lock and block self
            pthread_mutex_unlock(&action_lock);
            usleep(1000);   // sleep for 1ms, BUGBUG
        }

        // Pick up both semaphores now that we know both are available
        sem_wait(&acc_from->s);
        sem_wait(&acc_to->s);

        // Release action lock so while we are transferring someone else can try to do another
        pthread_mutex_unlock(&action_lock);

        // Do transfer
        acc_from->balance -= amount;
        acc_to->balance   += amount;

        // Pick up action lock
        pthread_mutex_lock(&action_lock);

        // Once obtained release both accounts and action lock
        sem_post(&acc_from->s);
        sem_post(&acc_to->s);

        pthread_mutex_unlock(&action_lock);

        // Reset buffer for next transfer
        memset(buffer, 0, MAX_STR_LEN);
    }

    pthread_exit(NULL);
}
