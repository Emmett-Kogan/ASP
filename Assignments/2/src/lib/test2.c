#include "FIFO.h"

FIFO_t foo;

static void *prod(void *arg) {
    int *arr = (int *) arg;
    for (int i = 0; i < 10; i++)
        FIFO_push(&foo, (void *) arr+(i*sizeof(int)));

    return NULL;
}

static void *cons(void *arg) {
    int rah;
    for (int i = 0; i < 10; i++) {
        FIFO_pop(&foo, (void *) &rah);
        printf("%d\n", rah);
    }
    return NULL;
}

int main() {
    FIFO_init(&foo, 10, sizeof(int));

    pthread_t t1, t2;
    void *res;
    int s;

    int arr[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    s = pthread_create(&t1, NULL, prod, (void *) arr);
    if (s != 0) perror("pthread_create 1");

    s = pthread_create(&t2, NULL, cons, NULL);
    if (s != 0) perror("pthread_create 2");

    printf("Message from main()\n");

    s = pthread_join(t1, &res);
    if (s != 0) perror("pthread_join 1");

    s = pthread_join(t2, &res);
    if (s != 0) perror("pthread_join 2");

    printf("Threads returned\n");

    return 0;
}
