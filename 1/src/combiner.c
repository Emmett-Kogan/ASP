#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

#include "wrline.h"

int main() {
    int pid_mapper, pid_reducer, return_status;
    int pfd1[2], pfd2[2];
    char buffer[32];

    // Setup pipes
    if (pipe(pfd1) == -1) perror("Pipe 1");
    if (pipe(pfd2) == -1) perror("Pipe 2");

    // Note that pipe one is written to by combiner and read by mapper
    // And pipe two is written to by mapper and read by reducer

    // Where mapper's fd 0 and 1 are pipes, and reducer's fd 0 is a pipe

    // Mapper
    pid_mapper = fork();
    switch(pid_mapper) {
        case -1:
            perror("fork 1");
        case 0:
            // Need to close write end of pipe 1 and read end of pipe 2
            close(pfd1[1]);
            close(pfd2[0]);

            // Replace std in and out with the pipes
            dup2(pfd1[0], 0);
            dup2(pfd2[1], 1);

            // Run the mapper program and kill the child process
            execl("mapper", "mapper", NULL);
            perror("mapper done");
            exit(0);
    }

    // Reducer
    pid_reducer = fork();
    switch(pid_reducer) {
        case -1:
            perror("fork 2");
        case 0:
            // Need to close both ends of pipe 1 and the write end of pipe 2
            close(pfd1[0]);
            close(pfd1[1]);
            close(pfd2[1]);

            // Replace std in with read end of pipe 2
            dup2(pfd2[0], 0);

            // Run the reducer program and kill the child process
            execl("reducer", "reducer", NULL);
            perror("reducer done");
            exit(0);
    }

    // Parent

    // Close unused ends of pipes
    close(pfd1[0]);
    close(pfd2[0]);
    close(pfd2[1]);

    // Take all input from stdin and write it to pipe 1 (to mapper)
    while(1) {
        if (readline(buffer, 24) == 0 || !buffer[0] || buffer[0] == '\n')
            break;
        writeline(pfd1[1], buffer);
        memset(buffer,0,32);
    }
    write(pfd1[1], "\n", 1);

    while(wait(NULL) > 0);

    return 0;
}
