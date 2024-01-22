#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "readline.h"

int main() {
	
	//test diff
    // fork, exec, pipe. dup2
    // Setup pipe
    int pfd1[2];
    int pfd2[2];
    pid_t pid;

    if (pipe(pfd1) == -1) perror("Pipe 1");
    if (pipe(pfd2) == -1) perror("Pipe 2");

    // Need 2 pipes
    // Pipe 1 is written to by combiner and read by mapper
    // Pipe 2 is written to by mapper and read by reducer
    // And reducer prints to stdout






    pid = fork();
    if (pid == -1)
        perror("Fork 1");

    // Mapper
    if (pid == 0) {
        dup2(pfd1[1], 0);   // Read end of pipe 1 to stdin
        dup2(pfd2[0], 1);   // Write end of pipe 2 to stdout

        // Close unused ends of pipes
        close(pfd1[0]);
        close(pfd2[1]);

        execl("mapper", "mapper",NULL);
        exit(0);
    }

    pid = fork();
    if (pid == -1) perror("Fork 2");

    // Reducer
    if (pid == 0) {
        dup2(pfd2[1], 0);   // Read end of pipe 2 to stdin

        // Close unused ends of pipes
        close(pfd1[0]);
        close(pfd1[1]);
        close(pfd2[0]);

        execl("reducer", "reducer", NULL);
        exit(0);
    }

    // Parent only

    // Close unused pipes
    close(pfd1[1]);
    close(pfd2[0]);
    close(pfd2[1]);

    // Copy all input from stdin and put in on the read end of pipe1
    char buffer[24];
    int temp = readline(buffer,24);
    while (temp > 0)
        write(pfd1[0], buffer, temp);

    return 0;
}
