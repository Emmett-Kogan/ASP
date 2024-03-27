#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    // Open
    int fd = open("/dev/tux0", O_RDWR);
    printf("/dev/tux0 fd = %d\n", fd);
    if (fd == -1)
    return -1;

    // Write
    int bytesWritten = write(fd, "Emmett Kogan\n", 14);
    printf("Wrote %d bytes to /dev/tux0\n", bytesWritten);

    // Read - used pread to provide offset, I think I had to do this
    // because lseek wasn't defined in the driver implementation
    printf("Printing what is read from the device: \n");
    char buffer[128];
    pread(fd, (void *) buffer, 14, 0);
    printf("\t%s\n", buffer);

    // Close
    close(fd);
    return 0;
}
