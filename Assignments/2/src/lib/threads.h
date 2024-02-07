#ifndef THREADS_H
#define THREADS_H

#include <stdio.h>
#include <string.h>

typedef struct tuple_t {
    int id;
    char topic[32];
    int value;
} tuple_t;

static void *reducer(void *args);

#endif
