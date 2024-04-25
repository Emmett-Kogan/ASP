# Assignment 6
## Contributor(s)
1. Emmett Kogan

## Build instructions
1. Use `make` to build the device driver. This will emit a kernel object file, `Assignment6Code.ko`, the needs to be loaded via `sudo insmod Assignment6Code.ko`. Then use `make app` to build the test executable, which takes a single command line arguement to specify which test you'd like to run. Between each test case, the device should be reloaded so that state from a previous test does not effect the following test (e.g. if you just caused deadlock, the state of the semaphores/wait_queues may not be obvious, and the tests were designed to initialize the state required anywyas).
2. I made a script, `test.sh`, to run each of the test cases, however, I tried a bunch of different ways to kill the deadlocked processes, and it works for some of the tests, but for test 2 I was having issues killing the processes I spawned... so uh, probably not best to use the script this time around.

## Test Cases
1. Test case 1 causes a deadlock using two processes and calling `e2_open()` and `e2_ioctl`. Let's say that process A opens the device (in `MODE1`). Then process B attempts `e2_open()`, and gets blocked when trying to aquire `sem2`, note that it would have already incremented `count1` however, so the current `count1` value is 2. Then if process A attempts to change the mode to `MODE2`, it will be waiting for `count1 > 1` to be false, which could only happen if process B releases (which is impossible). Therefore, this is deadlock. (The deadlock is techincally on the `down_interuptible()` on line 64 in `e2_open()`, and the `wait_event_interruptible()` in the `E2_IOCMODE2` case in `e2_ioctl()` on line 183)
2. Similairly to test case 1, in test case 2, if process A opens the device and changes the mode to `MODE2`, then process B opens the device. If both process A and process B then try to call `e2_ioctl()` and change to `MODE1`, then they will also deadlock (both need `count2 > 1` to be false, however, both can no longer close the device in order to change count2). In this case, the deadlock would occur with the `wait_event_interruptible()` on line 183. 
3. Test case 3 attempts to cause a deadlock by having a single process open the device, then spawn two threads. One of which writes a message to the ramdisk and reads it a lot, while another switches between modes to see if that causes a deadlock. Trying to see if the ioctl has a circular dependency with read.
4. Test case 4 does the same thing as 3, except there are two processes instead of two threads, and instead of toggling the mode one device is opening and closing the device in a loop.

## Race Condition Code Review
1. Let's say we have a process that opened the device with two threads in MODE1. Each thread (A & B) calls `e2_write()` for n bytes of data. A can run first, but be blocked after releasing the access control `sem1` but prior to calling `copy_from_user()`. While blocked, B can now both pick up, release and write to the ramdisk and "update" the file offset (see other notes). Once returned, A can then start again, and overwrite whatever B just wrote, so the final state of the buffer would be the message A wrote instead of the message B wrote, even though B was called second.T
2. The same class of problems as in `1`, a parent thread can enter the critical section of `e2_read()`, where it releases the semaphore prior to completing the read operation. It can be blocked, and now since `sem1` is available, another thread from the same process can call read. The main issue is that f_pos isn't being protected by `sem1` properly, so technically B could read bytes "out of bounds" since it isn't using the f_pos value after thread A's read call. BUT, the f_pos offset isn't being added to the basepointer in the copy_to_user calls so it's not like this would ever happen anyways.
3. In MODE1, if a `e2_read()` and `e2_write()` occur from a multithreaded process at the same time, each thread calls `up()` early in the MODE1 case so it doesn't protect access to the actual read/write operation and to f_pos. So it's possible for the 'critical sections' of read and write to run at concurently.
4. In `e2_write()`, if two processes attempt to call it at the same time, and the first process who gets sem1 is blocked after releasing sem1 but prior to modifying the ramdisk, then the second process can enter the critical section at the same time, and the f_pos is no longer mutually exclusive.

## Code Review
1. Choose between formatting styles: if following kernel programming style guide, follow all rules consistently. Including placement of curly braces and indentation of switch statements.
2. Choose either tabs for indentation OR spaces for indentation. Preferebly, configure tabs as spaces (and keep the number of spaces per indent CONSISTENT).
3. Defines should be all uppercase
4. Why is dev not static? majorNo, minorNo, and the class all are, and it's not like you need dev to be externally accessible.
5. Why on EARTH, if you have 1 device, did you not just statically allocate the structure. Why malloc it? Additionally, when you did call malloc, why did you call 2 mallocs back to back, for device struct and ramdisk. With the slab allocator, you will get pages very far away from each other this way. Do you hate your cache? Further, malloc and memset is simply WORSE than calloc. Please fix.
6. `down_interruptible` can spuriously return. All calls to `down_interuptible` should chcek the return value for ESYSRESTART in the event that this occurs.
7. Why do we allocate 64K bytes for a ramdisk, that we only ever access from the base pointer. Additionaly, the bounds checks with f_pos have no meaning because regardless of the the f_pos value, we always modify the same piece of memory. The bounds checks should only be checking the count.
8. Is the enter key on your keyboard broken? Or do you simply hate whitespace?
9. Any consistency in spacing of conditions and placement of parenthesis for ifs, functions and loops would be nice.
10. Why are some log messages using `pr_info()` and others using `printk()`?

TLDR: Use the emacs config posted on the linux kernel programming style guide, a linter, and compile with -Wall or else