#ifndef TYPEDEF_H
#define TYPEDEF_H

#define MAX_ARGS 128
#define MAX_ENV_VARS 128
#define MAX_ALIASES 64

typedef struct {
    char *name;
    char *value;
} EnvVar;

typedef struct {
    char *alias;
    char *command;
} Alias;

#endif // TYPEDEF_H
