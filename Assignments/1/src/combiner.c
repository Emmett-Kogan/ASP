// Author: Emmett Kogan
// Last modified: 2/4/24

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    int pid_mapper, pid_reducer, return_status;
    int pfd[2];
    char buffer[32];

    // Setup pipes
    if (pipe(pfd) == -1) perror("Pipe");

    // Mapper
    pid_mapper = fork();
    switch(pid_mapper) {
        case -1:
            perror("fork 1");
        case 0:
            // Need to close write end of pipe 1 and read end of pipe 2
            close(pfd[0]);

            // Replace std out with a pipe
            dup2(pfd[1], 1);

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
            // Need to close write end of pipe
            close(pfd[1]);

            // Replace std in with read end of pipe 2
            dup2(pfd[0], 0);

            // Run the reducer program and kill the child process
            execl("reducer", "reducer", NULL);
            perror("reducer done");
            exit(0);
    }

    // Parent
    close(pfd[0]);
    close(pfd[1]);

    while(wait(NULL) > 0);

    return 0;
}
