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
#include <pthread.h>
#include <errno.h>
#include <sys/wait.h>

#define CDRV_IOC_MAGIC 'Z'
#define E2_IOCMODE1 _IO(CDRV_IOC_MAGIC, 1)
#define E2_IOCMODE2 _IO(CDRV_IOC_MAGIC, 2)
#define ERR(s, n) { printf("%s", s); return n; }

// Test cases
static void testCase1(void);
static void testCase2(void);
static void testCase3(void);
static void testCase4(void);

// Threads
static void *test3thread1(void *args);
static void *test3thread2(void *args);

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

    switch(atoi(argv[1])) {
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

    // Note this should be unreachable
    while(wait(NULL) > 0);
}


// Note if you run this test case good luck killing the processes it runs

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
        close(fd);
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
        close(fd);
        break;
    }

    // Note this should be unreachable
    while(wait(NULL) > 0);
}

/*
 */
static void testCase3(void)
{
    int fd = open("/dev/a5", O_RDWR);
    printf("fd from main thread: %d\n", fd);
    void *res;
    pthread_t threads[2];
    pthread_create(&threads[0], NULL, test3thread1, &fd);
    pthread_create(&threads[1], NULL, test3thread2, &fd);
    pthread_join(threads[0], &res);
    pthread_join(threads[1], &res);
    close(fd);
}

/*
 */
static void testCase4(void)
{
    int fd, count = 0;

    // Rest of test case
    switch(fork()) {
    case 0:
    {
        // Child
        fd = open("/dev/a5", O_RDWR);
        char buffer[32];

        while(1) {
            sleep(0.05);
            write(fd, "test string", 11);
            read(fd, buffer, 11);
            if (count++ > 200) break;
        }

        exit(0);
    }
    default:
        // Parent
        while(1) {
            sleep(0.05);
            fd = open("/dev/a5", O_RDWR);
            close(fd);
            if (count++ > 200) break;
        }
    }

    while(wait(NULL) > 0);

}

static void *test3thread1(void *args)
{
    // thread 1:
        // does a write, and then while(1) { read(); }
    int fd = *((int *) args), count = 0;
    char buffer[32];

    printf("fd from thread1 %d\n", fd);

    printf("%ld\n", write(fd, "test string", 11));
    while(1) {
        sleep(0.05);
        read(fd, buffer, 11);
        printf("Thread1: %s\n", buffer);
        if (count++ > 200) break;
    }

    pthread_exit(NULL);
}

static void *test3thread2(void *args)
{

    // thread 2:
        // while(1) { ioctl(to mode1); ioctl(to mode2); }
    int fd = *((int *) args), count = 0;

        printf("fd from thread2 %d\n", fd);
    while(1) {
        ioctl(fd, E2_IOCMODE2);
        sleep(0.05);
        ioctl(fd, E2_IOCMODE1);
        if (count++ > 200) break;
    }

    pthread_exit(NULL);
}