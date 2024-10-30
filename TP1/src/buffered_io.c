#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "buffered_io.h"

#define BUFFER_SIZE 1024

MY_FILE* my_open(const char* filename, const char* mode) {
    int fd;
    if (strcmp(mode, "r") == 0) {
        fd = open(filename, O_RDONLY);
    } else if (strcmp(mode, "w") == 0) {
        fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    } else {
        return NULL;  
    }

    if (fd == -1) {
        perror("Error opening file");
        return NULL;
    }

    MY_FILE* file = (MY_FILE*) malloc(sizeof(MY_FILE));
    if (!file) {
        perror("Error allocating memory");
        close(fd);
        return NULL;
    }

    file->fd = fd;
    file->buf_pos = 0;
    file->buf_size = 0;
    return file;
}

int my_getc(MY_FILE* file) {
    if (file->buf_pos >= file->buf_size) {
        file->buf_size = read(file->fd, file->buffer, BUFFER_SIZE);
        file->buf_pos = 0;
        if (file->buf_size == 0) { 
            return EOF;
        } else if (file->buf_size < 0) {  
            perror("Error reading file");
            return EOF;
        }
    }

    return file->buffer[file->buf_pos++];
}


int my_putc(int c, MY_FILE* file) {
    if (file->buf_pos >= BUFFER_SIZE) {
        if (write(file->fd, file->buffer, BUFFER_SIZE) != BUFFER_SIZE) {
            perror("Error writing to file");
            return EOF;
        }
        file->buf_pos = 0;
    }

    file->buffer[file->buf_pos++] = (char) c;
    return c;
}

int my_close(MY_FILE* file) {
    if (file->buf_pos > 0) {
        ssize_t bytes_written = write(file->fd, file->buffer, file->buf_pos);
        if (bytes_written != file->buf_pos) {
            perror("Error writing final buffer");
            close(file->fd);
            free(file);
            return -1;

        }
    }
    int result = close(file->fd);
    free(file);
    return result;
}
