// Author: Emmett Kogan
// Last modified: 2/11/24

#ifndef THREADS_H
#define THREADS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "FIFO.h"

static void *worker(void *args) {

    pthread_exit(0);
}

#endif
