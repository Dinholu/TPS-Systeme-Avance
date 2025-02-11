#include "include/history.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define HISTORY_FILE "history.txt"

void add_to_history(const char *command) {
  if (!command) {
      errno = EFAULT;
      return;
  }
  
  int fd = open(HISTORY_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
  if (fd == -1) {
    switch (errno) {
        case EACCES: perror("add_to_history: Permission denied"); break;
        case EISDIR: perror("add_to_history: Is a directory"); break;
        case ENOENT: perror("add_to_history: No such file or directory"); break;
        case EFAULT: perror("add_to_history: Bad address"); break;
        default: perror("add_to_history: Failed to open history file"); break;
    }
    return;
  }

  if (write(fd, command, strlen(command)) == -1 || write(fd, "\n", 1) == -1) {
        switch (errno) {
            case EFAULT: perror("add_to_history: Bad address"); break;
            default: perror("add_to_history: Fail to write to history file"); break;
        }
  }

  close(fd);
}

void display_history() {
  FILE *file = fopen(HISTORY_FILE, "r");
  if (!file) {
    switch (errno) {
        case EACCES: perror("display_history: Permission denied"); break;
        case ENOENT: perror("display_history: No such file or directory"); break;
        default: perror("display_history: Failed to open history file"); break;
    }
    return;
  }

  char line[1024];
  int line_number = 1;
  while (fgets(line, sizeof(line), file)) {
    printf("%d: %s", line_number++, line);
  }

  fclose(file);
}
