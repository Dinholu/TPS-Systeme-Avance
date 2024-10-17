#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "reverse_print.h"

void print_reverse(const char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1) {
        perror("Error determining file size");
        close(fd);
        exit(EXIT_FAILURE);
    }

    char ch;
    for (off_t i = file_size - 1; i >= 0; --i) {
        if (lseek(fd, i, SEEK_SET) == -1) {
            perror("Error seeking in file");
            close(fd);
            exit(EXIT_FAILURE);
        }

        if (read(fd, &ch, 1) == -1) {
            perror("Error reading file");
            close(fd);
            exit(EXIT_FAILURE);
        }

        write(STDOUT_FILENO, &ch, 1);
    }

    close(fd);
}
