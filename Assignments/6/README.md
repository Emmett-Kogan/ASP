# Assignment 5
## Contributor(s)
1. Emmett Kogan

## Build instructions
1. Run `make` to build the driver, run `make app` to build `devuserapp` to test the driver with. Make clean removes all created/temporary files.
2. Once `char_driver.ko` has been built, run `sudo insmod char_driver.ko` to load the driver. Note that there are params `majorno`, `numdevices`, and, `size` which can be used to modify the default values of the driver (500, 3, 16*PAGE_SIZE respectivley).
3. Now, running `devuserapp` with any of the input files in `testfiles/` as a cmd line arguement will test the driver.

I also provided a script, `test.sh`, that does all of this, and runs and diffs each of the provided test cases, just with the default params of the driver.
I modified the devuserapp.cpp file to fix the typo: mycdrv -> mycdev, but that's it I think.

## Test Cases
1. If in mode 2, and more than one process is attempting to switch to mode 1 at a time, then they deadlock. Both are waiting infinetley on the other to give up
2. Open in mode1, then when another process tries to open while still in mode1, if the former does any operation involving sem1 (all operations) then deadlock

Process A opens and operates in MODE1. Process B attempts to open, and gets blocked waiting for sem2 (does not hold sem1).
3. 
4. 

Matthew's Ideas
Switching from mode 2 to 1 while both threads are accessing it
writing invalid length and then a read after it?
switching from mode1 to 2 while another thread tries to open it
write in one thread read in another?

## Race Condition Code Review
1. Let's say we have a process that opened the device with two threads. Each thread (A & B) calls `e2_write()` for n bytes of data. A runs first, completes its copy_from_user call, and then blocks prior to updating the offset. B then runs to completion, using the same original offset as A. Then both A and B add the count of bytes they wrote to the file position. This would lose the data written by one of the threads, as well as signal that n+m bytes were written, when in reality only n or m bytes were written. The data within the critical region is the ramdisk and file offset, and the lock held on entry was sem1, however, it was prematurley released.
2. The same class of problems as in `1`, a parent thread can enter the critical section of `e2_read()`, where it releases the semaphore prior to completing the read operation. It can be blocked, and now since `sem1` is available, another thread from the same process can call read. Lets say that the first thread blocks after the copy_to_user but before the file position is updated. Then thread B does the exact same thing. So that both A and B have read n bytes (the same n bytes as they started with the same file position) and now collectivley add 2n to the file position, possibly missing out on n bytes of data. The data within the critical region is the ramdisk and file offset, and the lock held on entry was sem1, however, it was prematurley released.
3. 
4. 