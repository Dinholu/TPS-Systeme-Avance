#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "typedef.h"

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
        args[i++] = strdup(token);
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    free(cmd_copy);
    return args;
}
