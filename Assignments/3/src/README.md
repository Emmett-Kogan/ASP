# Assignment 3
## Contributor(s)
1. Emmett Kogan

## Notes
1. Will just use the same FIFO struct from before to talk between the threads, I think it's just a main thread that assigns work to EFT worker threads
2. There will also need to be a global variable/struct for each account, this will be initialized in the main thread so it will probably need to be a linked list of nodes with an id and an int value for balance
3. Once a line is found that starts with "T" its the start of the transfer stage, where the main thread goes from initializing the nodes to distributing commands for each thread in a round robin maner, so probably just a counter that is modded by the max threads that's incremented after each command, and an array of threads and FIFOs just like with assignment 2
4. So then once all of the transfers have been given we wait for each thread to finish, I'll have the same system of a newline character in the first spot to denote we are done, and once it is all done the threads will all return, and the main thread will print the final value of the accounts
5. The threads themselves will need to be able to acquire exclusive access to 2 accounts without becoming deadlocked
6. Now the assignment was made with using semaphores in mind, but I think I'd end up just doing binary semaphores anyways, so doing this mutexes might be better, where I take advantage of pthread_mutex_trylock on the second one, and if that fails unlock the first one and try again/sleep and try again.
7. So that would mean that each account in the linked list would need a mutex, so the init process should be getting the linked list, once I have the number of accounts dynamically making the mutexes, (and I guess have those as global variables?) and then turn it over to the worker threads to mess with them after that.

## Build Instructions
1. Do `make` as the default rule `transfer` (`clean` deletes all produced files if you want to clean up)
2. 
