#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include "../src/posix4dos.h"

#define TEST_FILENAME "test_file.txt"
#define TEST_STRING "Hello from posix4dos!
"

int main() {
    int fd;
    ssize_t bytes_written, bytes_read;
    char read_buf[50];
    off_t seek_pos;

    printf("--- posix4dos Example Program ---
");

    // 1. Create and open a file for writing
    printf("Opening %s for writing...
", TEST_FILENAME);
    fd = p4d_open(TEST_FILENAME, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
        perror("p4d_open for writing failed");
        return EXIT_FAILURE;
    }
    printf("File opened with fd: %d
", fd);

    // 2. Write to the file
    printf("Writing "%s" to %s...
", TEST_STRING, TEST_FILENAME);
    bytes_written = p4d_write(fd, TEST_STRING, strlen(TEST_STRING));
    if (bytes_written == -1) {
        perror("p4d_write failed");
        p4d_close(fd);
        return EXIT_FAILURE;
    }
    printf("Wrote %ld bytes.
", bytes_written);

    // 3. Close the file
    printf("Closing %s...
", TEST_FILENAME);
    if (p4d_close(fd) == -1) {
        perror("p4d_close failed after writing");
        return EXIT_FAILURE;
    }
    printf("File closed.
");

    // 4. Open the file for reading
    printf("Opening %s for reading...
", TEST_FILENAME);
    fd = p4d_open(TEST_FILENAME, O_RDONLY);
    if (fd == -1) {
        perror("p4d_open for reading failed");
        return EXIT_FAILURE;
    }
    printf("File opened with fd: %d
", fd);

    // 5. Seek to the beginning of the file
    printf("Seeking to beginning of file...
");
    seek_pos = p4d_lseek(fd, 0, SEEK_SET);
    if (seek_pos == (off_t)-1) {
        perror("p4d_lseek failed");
        p4d_close(fd);
        return EXIT_FAILURE;
    }
    printf("Current file position: %ld
", seek_pos);

    // 6. Read from the file
    printf("Reading from %s...
", TEST_FILENAME);
    memset(read_buf, 0, sizeof(read_buf)); // Clear buffer
    bytes_read = p4d_read(fd, read_buf, sizeof(read_buf) - 1); // -1 for null terminator
    if (bytes_read == -1) {
        perror("p4d_read failed");
        p4d_close(fd);
        return EXIT_FAILURE;
    }
    printf("Read %ld bytes: "%s"
", bytes_read, read_buf);

    // 7. Close the file
    printf("Closing %s...
", TEST_FILENAME);
    if (p4d_close(fd) == -1) {
        perror("p4d_close failed after reading");
        return EXIT_FAILURE;
    }
    printf("File closed.
");

    printf("--- Example Program Finished ---
");
    return EXIT_SUCCESS;
}
