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

int main() 
{
    int fd; 

    switch(fork()) {
    case 0:
        fd = open("/dev/a5", O_RDWR);
        printf("child fd = %d\n", fd);
        close(fd);
        return 0;
    default:
        fd = open("/dev/a5", O_RDWR);
        printf("parent fd = %d\n", fd);
        break;
    }

    close(fd);

    return 0;
}
