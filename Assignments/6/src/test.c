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
#include <errno.h>
#include <sys/wait.h>

#define CDRV_IOC_MAGIC 'Z'
#define E2_IOCMODE1 _IO(CDRV_IOC_MAGIC, 1)
#define E2_IOCMODE2 _IO(CDRV_IOC_MAGIC, 2)

#define ERR(s, n) { printf("%s", s); return n; }

static void testCase1(void);
static void testCase2(void);
static void testCase3(void);
static void testCase4(void);


    // int fd = open("/dev/a5", O_RDWR);

    // switch(fork()) {
    // case 0:
    //     // Child

    //     return 0;
    // default:
    //     break;        
    // }

    // pthread_t thread;
    // pthread_create(&thread, NULL, worker, &fd);

    

    // void *res;
    // pthread_join(thread, &res);
    // close(fd);


static void *worker(void *args)
{   
    int fd = *((int *) args);
    int res = write(fd, ", World!", 8);
    printf("Bytes written from child: %d\n", res);
    pthread_exit(NULL);
}

int main(int argc, char **argv) 
{
    if (argc < 2)
        ERR("Bad command line args\n", -EINVAL);

    int testNo = atoi(argv[1]);

    switch(testNo) {
    case 1:
        testCase1();
        break;
    case 2:
        testCase2();
        break;
    case 3:
        testCase3();
        break;
    case 4:
        testCase4();
        break;
    default:
        ERR("Invalid test case\n", -EINVAL);
    }

    return 0;
}

/* Test case 1 causes a deadlock using two processes and calling `e2_open()` and `e2_ioctl`. 
 * Let's say that process A opens the device (in `MODE1`). Then process B attempts `e2_open()`,
 * and gets blocked when trying to aquire `sem2`, note that it would have already incremented
 * `count1` however, so the current `count1` value is 2. Then if process A attempts to change 
 * the mode to `MODE2`, it will be waiting for `count1 > 1` to be false, which could only happen 
 * if process B releases (which is impossible). Therefore, this is deadlock. (The deadlock is techincally 
 * on the `down_interuptible()` on line 64 in `e2_open()`, and the `wait_event_interruptible()` in the 
 * `E2_IOCMODE2` case in `e2_ioctl()` on line 183)
 */
static void testCase1(void)
{  
    int fd; 
 
    switch(fork()) {
    case 0:
        // Child
        sleep(1);
        printf("Child opening device\n");
        fd = open("/dev/a5", O_RDWR);
        printf("Child not stuck on sem2?!?\n");

        // Fork will call open on this so, count1 is 2, and this is blocked on the down_interruptible(sem2) in e2_open
        close(fd);
        exit(0);
    default:
        // Parent
        printf("Parent opening device\n");
        fd = open("/dev/a5", O_RDWR);
        sleep(2);
        printf("Parent attempting to switch to MODE2\n");
        ioctl(fd, E2_IOCMODE2);    // this should cause deadlock
        close(fd);
    }

}

/* Similairly to test case 1, in test case 2, if process A opens the device and changes the 
 * mode to `MODE2`, then process B opens the device. If both process A and process B then try to call 
 * `e2_ioctl()` and change to `MODE1`, then they will also deadlock (both need `count2 > 1` to be false, 
 * however, both can no longer close the device in order to change count2). In this case, the deadlock would 
 * occur with the `wait_event_interruptible()` on line 183. 
 */
static void testCase2(void)
{
    int fd;
    switch(fork()) {
    case 0:
        // Child
        sleep(1);
        printf("Child attempting to open the device\n");
        fd = open("/dev/a5", O_RDWR);
        printf("Child attempting to switch to MODE1\n");
        ioctl(fd, E2_IOCMODE1);

        exit(0);
    default:
        // Parent
        printf("Parent attempting to open the device\n");
        fd = open("/dev/a5", O_RDWR);
        printf("Parent attempting to switch to MODE2\n");
        ioctl(fd, E2_IOCMODE2);
        sleep(5);
        printf("Parent attempting to switch to MODE1\n");
        ioctl(fd, E2_IOCMODE1);
        break;
    }
}

/*
 */
static void testCase3(void)
{
    
}

/*
 */
static void testCase4(void)
{
    // Le funny
    int fd = open("/dev/a5", O_RDWR);

    for (int i = 0; i < 10; i++) {
        printf("Forking\n");
        switch(fork()) {
        case 0:
            printf("Proc%d close returned %d\n", i, close(fd));
            exit(0);
        default:
            continue;
        }
    }

    while(wait(NULL) > 0);
}



