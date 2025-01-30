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
    // Reset buffer for each command
    buf_index = 0;

    while (*ptr == ' ')
      ptr++;

    // Read until a control operator or end of input
    while (*ptr && !((*ptr == '&' && *(ptr + 1) == '&') ||
                     (*ptr == '|' && *(ptr + 1) == '|'))) {
      buffer[buf_index++] = *ptr++;
    }

    buffer[buf_index] = '\0'; // Null-terminate the command string
    commands[count].command = strdup(buffer);

    while (commands[count].command[strlen(commands[count].command) - 1] ==
           ' ') {
      commands[count].command[strlen(commands[count].command) - 1] = '\0';
    }

    // Detect control operator type
    if (*ptr == '&' && *(ptr + 1) == '&') {
      commands[count].type = CMD_AND;
      ptr += 2;
    } else if (*ptr == '|' && *(ptr + 1) == '|') {
      commands[count].type = CMD_OR;
      ptr += 2;
    } else {
      commands[count].type = CMD_NONE;
    }

    count++;
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
