#include "include/history.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define HISTORY_FILE "history.txt"

void add_to_history(const char *command) {
  int fd = open(HISTORY_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
  if (fd == -1) {
    perror("Erreur lors de l'ouverture du fichier d'historique");
    return;
  }

  if (write(fd, command, strlen(command)) == -1 || write(fd, "\n", 1) == -1) {
    perror("Erreur lors de l'écriture dans le fichier d'historique");
  }

  close(fd);
}

void display_history() {
  FILE *file = fopen(HISTORY_FILE, "r");
  if (!file) {
    perror("Erreur lors de l'ouverture du fichier d'historique");
    return;
  }

  char line[1024];
  int line_number = 1;
  while (fgets(line, sizeof(line), file)) {
    printf("%d: %s", line_number++, line);
  }

  fclose(file);
}
