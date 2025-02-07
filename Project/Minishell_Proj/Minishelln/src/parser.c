#include "include/parser.h"
#include "include/typedef.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int parse_control_operators(const char *input, CommandNode commands[]) {
  if (!input || !commands) {
    errno = EFAULT;
    return -1;
  }
  
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
        errno = ENOMEM;
        return -1;
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
  if (!command) {
    errno = EFAULT;
    return NULL;
  }

  char **args = malloc(MAX_ARGS * sizeof(char *));
  if (!args) {
    errno = ENOMEM;
    return NULL;
  }

  char *cmd_copy = strdup(command);
  if (!cmd_copy) {
    free(args);
    errno = ENOMEM;
    return NULL;
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
      free(cmd_copy);
      for (int j = 0; j < i; j++) {
        free(args[j]);
      }
      free(args);
      errno = ENOMEM;
      return NULL;
    }
    i++;
    token = strtok(NULL, " ");
  }
  args[i] = NULL;

  free(cmd_copy);
  return args;
}

char **split_pipes(const char *command) {
  if (!command) {
    errno = EFAULT;
    return NULL;
  }

  char **segments = malloc(MAX_PIPE_SEGMENTS * sizeof(char *));
  if (!segments) {
    errno = ENOMEM;
    return NULL;
  }

  char *cmd_copy = strdup(command);
  if (!cmd_copy) {
    free(segments);
    errno = ENOMEM;
    return NULL;
  }

  char *token = strtok(cmd_copy, "|");
  int i = 0;
  while (token && i < MAX_PIPE_SEGMENTS - 1) {
    segments[i] = strdup(token);
    if (!segments[i]) {
      free(cmd_copy);
      for (int j = 0; j < i; j++) {
        free(segments[j]);
      }
      free(segments);
      errno = ENOMEM;
      return NULL;
    }
    i++;
    token = strtok(NULL, "|");
  }
  segments[i] = NULL;

  free(cmd_copy);
  return segments;
}

int is_background_command(char **args) {
  if (!args) {
    errno = EFAULT;
    return -1;
  }

  for (int i = 0; args[i]; i++) {
    if (strcmp(args[i], "&") == 0) {
      free(args[i]);
      args[i] = NULL;
      return 1;
    }
  }
  return 0;
}
