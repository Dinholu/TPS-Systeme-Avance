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

    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("Error repositioning file");
        close(fd);
        exit(EXIT_FAILURE);
    }

    char *buffer = (char *)malloc(file_size);
    if (buffer == NULL) {
        perror("Memory allocation error");
        close(fd);
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_read = read(fd, buffer, file_size);
    if (bytes_read == -1) {
        perror("Error reading file");
        free(buffer);
        close(fd);
        exit(EXIT_FAILURE);
    }

    for (off_t i = file_size - 1; i >= 0; --i) {
        write(STDOUT_FILENO, &buffer[i], 1);
    }

    write(STDOUT_FILENO, "\n", 1);

    free(buffer);
    close(fd);
}
