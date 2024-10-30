#ifndef BUFFERED_IO_H
#define BUFFERED_IO_H

typedef struct {
    int fd;
    char buffer[1024];  // Buffer size
    int buf_pos;        // Buffer position
    int buf_size;       // Buffer size now
} MY_FILE;

MY_FILE* my_open(const char* filename, const char* mode);
int my_getc(MY_FILE* file);
int my_putc(int c, MY_FILE* file);
int my_close(MY_FILE* file);

#endif
