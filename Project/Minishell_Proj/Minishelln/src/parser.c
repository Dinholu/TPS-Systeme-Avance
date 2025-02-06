#include "include/parser.h"
#include "include/typedef.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_control_operators(const char *input, CommandNode commands[]) {
  int count = 0;
  const char *ptr = input;
  char buffer[1024];
  int buf_index = 0;

  while (*ptr && count < MAX_COMMANDS) {
    // Reset du buffer
    buf_index = 0;

    while (*ptr == ' ') {
      ptr++;
    }

    // Lis la commande jusqu'à trouver un opérateur de contrôle
    while (*ptr && strncmp(ptr, "&&", 2) != 0 && strncmp(ptr, "||", 2) != 0) {
      if (buf_index < (int)(sizeof(buffer) - 1)) {
        buffer[buf_index++] = *ptr;
      }
      ptr++;
    }

    buffer[buf_index] = '\0';
    if (buf_index > 0) {
      commands[count].command = strdup(buffer);
      if (!commands[count].command) {
        perror("strdup");
        exit(EXIT_FAILURE);
      }
      commands[count].type = CMD_NONE;
      count++;
    }

    // Détection de l'opérateur de contrôle
    if (strncmp(ptr, "&&", 2) == 0) {
      commands[count - 1].type = CMD_AND;
      ptr += 2;
    } else if (strncmp(ptr, "||", 2) == 0) {
      commands[count - 1].type = CMD_OR;
      ptr += 2;
    }
  }

  return count;
}

char **parse_command(const char *command) {
  char **args = malloc(MAX_ARGS * sizeof(char *));
  if (!args) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  char *cmd_copy = strdup(command);
  if (!cmd_copy) {
    perror("strdup");
    free(args);
    exit(EXIT_FAILURE);
  }

  char *token = strtok(cmd_copy, " ");
  int i = 0;
  while (token && i < MAX_ARGS - 1) {
    if (token[0] == '"' && token[strlen(token) - 1] == '"') {
      token[strlen(token) - 1] = '\0';
      token++;
    }
    args[i] = strdup(token);
    if (!args[i]) {
      perror("strdup");
      free(cmd_copy);
      for (int j = 0; j < i; j++) {
        free(args[j]);
      }
      free(args);
      exit(EXIT_FAILURE);
    }
    i++;
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
    free(segments);
    exit(EXIT_FAILURE);
  }

  char *token = strtok(cmd_copy, "|");
  int i = 0;
  while (token && i < MAX_PIPE_SEGMENTS - 1) {
    segments[i] = strdup(token);
    if (!segments[i]) {
      perror("strdup");
      free(cmd_copy);
      for (int j = 0; j < i; j++) {
        free(segments[j]);
      }
      free(segments);
      exit(EXIT_FAILURE);
    }
    i++;
    token = strtok(NULL, "|");
  }
  segments[i] = NULL;

  free(cmd_copy);
  return segments;
}

int is_background_command(char **args) {
  for (int i = 0; args[i]; i++) {
    if (strcmp(args[i], "&") == 0) {
      free(args[i]);
      args[i] = NULL;
      return 1;
    }
  }
  return 0;
}
