#ifndef FIFO_H
#define FIFO_H

#include <pthread.h>
#include <stdint.h>

/*
 In the case of assignment 2, I need a FIFO struct that I can make variably long, so probably using the head since I'm getting the info of how long at compile time,

 The actual array/buffer whatever will be hidden from the user since there will be FIFO_push and FIFO_pop as pretty much the only way of interfacing with it


 */


typedef struct FIFO_t {
    uint32_t size;

} FIFO_t;



#endif
