# Assignment 1
## Contributor(s)
Emmett Kogan

## TODO:
1. Test FIFO struct and functions to make sure it actually works with just one process, then make a multithreaded test
2. Dealing with the actual assignment as well lol
## Build instructions

## Scratch
Talks about using some buffer data structure to pass input from the main/mapper thread to the reducer/user threads.

I'm thinking of just a generic FIFO implementation that has a read mutex and where count is a counting semaphore. Would need a head and tail pointer for this to do it in a circular array, and I would also like to have count be a counting semaphore so that when a thread attempts to read an empty FIFO, it is blocked

Also note I need to use the pthread version of these functions