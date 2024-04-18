#include <linux/ioctl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>

static void *worker(void *args)
{   
    int fd = *((int *) args);
    int res = write(fd, ", World!", 8);
    printf("Bytes written from child: %d\n", res);
    pthread_exit(NULL);
}

int main() 
{

    int fd = open("/dev/a5", O_RDWR);

    switch(fork()) {
    case 0:
        // Child

        return 0;
    default:
        break;        
    }

    pthread_t thread;
    pthread_create(&thread, NULL, worker, &fd);

    

    void *res;
    pthread_join(thread, &res);
    close(fd);

    return 0;
}