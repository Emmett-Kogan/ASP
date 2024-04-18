# Assignment 5
## Contributor(s)
1. Emmett Kogan

## Build instructions

## Test Cases
1. If in mode 2, and more than one process is attempting to switch to mode 1 at a time, then they deadlock. Both are waiting infinetley on the other to give up
Process A opens the device in MODE1 and successfully finishes the open call
Process B tries to open the device in MODE1 and is blocked on the down(sem2) in the mode1 case
count1 = 2;


Process A tries to switches to mode 2 but count1 will never change from 2 as long as this runs
So Process B will be forever blocked on sem2, and process A will be forever waiting for count1 to be decremented
2. 

3. 
4. 

## Race Condition Code Review
1. Let's say we have a process that opened the device with two threads in MODE1. Each thread (A & B) calls `e2_write()` for n bytes of data. A can run first, but be blocked after releasing the access control `sem1` but prior to calling `copy_from_user()`. While blocked, B can now both pick up, release and write to the ramdisk and "update" the file offset (see other notes). Once returned, A can then start again, and overwrite whatever B just wrote, so the final state of the buffer would be the message A wrote instead of the message B wrote, even though B was called second. Technically, the f_pos value is not being protected correctly by `sem1`. But, it isn't even being added to the base pointer to ramdisk anyways so it's not like the placement of either message would be different regardless.
2. The same class of problems as in `1`, a parent thread can enter the critical section of `e2_read()`, where it releases the semaphore prior to completing the read operation. It can be blocked, and now since `sem1` is available, another thread from the same process can call read. The main issue is that f_pos isn't being protected by `sem1` properly, so technically B could read bytes "out of bounds" since it isn't using the f_pos value after thread A's read call. BUT, the f_pos offset isn't being added to the basepointer in the copy_to_user calls so it's not like this would ever happen anyways.
3. In MODE1, if a `e2_read()` and `e2_write()` occur from a multithreaded process at the same time, each thread calls `up()` early in the MODE1 case so it doesn't protect access to the actual read/write operation and to f_pos. So it's possible for the 'critical sections' of read and write to run at concurently.
4. 







## Other Notes

To summarize, Yavuz handed us a file that _doesn't_ compile. That when you fix _doesn't_ install. And when you fix that, it allocates **16 PAGES** of memory, of which it uses like, the first n bytes (where n is the length of any write) because in `e2_read()` and `e2_write()`, it _doens't_ add the fucking offset to the base pointer of the buffer when calling `copy_to_user()` and `copy_from_user()`. Which means that any read or write is essentially independent of one another, except a bounds check on an offset that **isn't even used**. 