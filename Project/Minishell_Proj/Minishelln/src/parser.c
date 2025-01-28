#include "include/typedef.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **parse_command(const char *command) {
  char **args = malloc(MAX_ARGS * sizeof(char *));
  if (!args) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  char *cmd_copy = strdup(command);
  if (!cmd_copy) {
    perror("strdup");
    exit(EXIT_FAILURE);
  }

  char *token = strtok(cmd_copy, " ");
  int i = 0;
  while (token && i < MAX_ARGS - 1) {
    if (token[0] == '"' && token[strlen(token) - 1] == '"') {
      token[strlen(token) - 1] = '\0'; // Supprime le guillemet de fin
      token++;                         // Supprime le guillemet de début
    }
    args[i++] = strdup(token);
    token = strtok(NULL, " ");
  }
  args[i] = NULL;

  free(cmd_copy);
  return args;
}

char **split_pipes(const char *command) {
  char **segments = malloc(MAX_PIPE_SEGMENTS * sizeof(char *));
  if (!segments) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  char *cmd_copy = strdup(command);
  if (!cmd_copy) {
    perror("strdup");
    exit(EXIT_FAILURE);
  }

  char *token = strtok(cmd_copy, "|");
  int i = 0;
  while (token && i < MAX_PIPE_SEGMENTS - 1) {
    segments[i++] = strdup(token);
    token = strtok(NULL, "|");
  }
  segments[i] = NULL;

  free(cmd_copy);
  return segments;
}
