#include "FIFO.h"

FIFO_t foo;

static void *prod(void *arg) {
    char *str = (char *) arg;
    while(*str) FIFO_push(&foo, (void *) str++);
    return NULL;

}

static void *cons(void *arg) {
    char rah;
    while(1) {
        FIFO_pop(&foo, (void *) &rah);
        printf("%c", rah);
        if (!rah || rah == '\n') break;
    }
    return NULL;
}

int main() {
    FIFO_init(&foo, 40, 1);

    pthread_t t1, t2;
    void *res;
    int s;

    s = pthread_create(&t1, NULL, prod, "Hello, World!\n");
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
