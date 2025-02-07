#include "include/typedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

// Commandes intégrées (built-in)
int builtin_cd(char **args) {
    if (!args) {
        errno = EFAULT;
        return -1;
    }

    if (!args[1]) {
        perror("cd: missing argument\n");
        return -1;
    }
    if (chdir(args[1]) != 0) {
        switch (errno) {
            case EFAULT: perror("cd: Bad address\n"); break;
            case ENOTDIR: perror("cd: Not a directory\n"); break;
            case ELOOP: perror("cd: Too many symbolic links\n"); break;
            default: perror("cd"); break;
        }
        return -1;
    }
    return 0;
}

int builtin_pwd() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        switch (errno) {
            case EFAULT: perror("pwd: Bad address\n"); break;
            case ERANGE: perror("pwd: Buffer too small\n"); break;
            default: perror("pwd"); break;
        }
        return -1;
    }
    printf("%s\n", cwd);
    return 0;
}

int builtin_echo(char **args) {
    if (!args) {
        errno = EFAULT;
        return -1;
    }

    for (int i = 1; args[i]; i++) {
        if (args[i][0] == '"' && args[i][strlen(args[i]) - 1] == '"') {
            args[i][strlen(args[i]) - 1] = '\0';
            printf("%s", args[i] + 1);
        } else {
            printf("%s", args[i]);
        }
        if (args[i + 1]) {
            printf(" ");
        }
    }
    printf("\n");
    return 0;
}

int builtin_exit() {
    printf("Exiting shell...\nThanks to have used it ! Bye !\n");
    exit(0);
}

// Variables d'environnement
EnvVar env_vars[MAX_ENV_VARS];
int env_count = 0;

int find_env_var(const char *name) {
    if (!name) {
        errno = EFAULT;
        return -1;
    }

    for (int i = 0; i < env_count; i++) {
        if (strcmp(env_vars[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void set_env_var(const char *name, const char *value) {
    if (!name || !value) {
        errno = EFAULT;
        return;
    }

    int index = find_env_var(name);
    if (index >= 0) {
        free(env_vars[index].value);
        if ((env_vars[index].value = strdup(value)) == NULL) {
            perror("set_env_var: Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
    } else {
        if (env_count < MAX_ENV_VARS) {
            if ((env_vars[env_count].name = strdup(name)) == NULL ||
                (env_vars[env_count].value = strdup(value)) == NULL) {
                perror("set_env_var: Memory allocation failed\n");
                exit(EXIT_FAILURE);
            }
            env_count++;
        } else {
            perror("Maximum environment variable limit reached.\n");
        }
    }
}

char *get_env_var(const char *name) {
    if (!name) {
        errno = EFAULT;
        return NULL;
    }

    int index = find_env_var(name);
    return index >= 0 ? env_vars[index].value : NULL;
}

void unset_env_var(const char *name) {
    if (!name) {
        errno = EFAULT;
        return;
    }

    int index = find_env_var(name);
    if (index >= 0) {
        free(env_vars[index].name);
        free(env_vars[index].value);
        for (int i = index; i < env_count - 1; i++) {
            env_vars[i] = env_vars[i + 1];
        }
        env_count--;
    }
}

void expand_env_variables(char **args) {
    if (!args) {
        errno = EFAULT;
        return;
    }

    for (int i = 0; args[i]; i++) {
        if (args[i][0] == '$' && strlen(args[i]) > 1) {
            char *var_name = args[i] + 1;
            char *value = get_env_var(var_name);
            if (value) {
                free(args[i]);
                if ((args[i] = strdup(value)) == NULL) {
                    perror("expand_env_variables: Memory allocation failed\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
}

int builtin_env() {
    for (int i = 0; i < env_count; i++) {
        printf("%s=%s\n", env_vars[i].name, env_vars[i].value);
    }
    if (env_count == 0) {
        printf("No environment variables defined.\n");
    }
    return 0;
}

// Gestion des alias
Alias aliases[MAX_ALIASES];
int alias_count = 0;

void set_alias(const char *alias, const char *command) {
    if (!alias || !command) {
        errno = EFAULT;
        return;
    }

    if (strcmp(alias, command) == 0) {
        perror("alias: cannot create recursive alias\n");
        return;
    }

    for (int i = 0; i < alias_count; i++) {
        if (strcmp(aliases[i].alias, alias) == 0) {
            free(aliases[i].command);
            if ((aliases[i].command = strdup(command)) == NULL) {
                perror("set_alias: Memory allocation failed\n");
                exit(EXIT_FAILURE);
            }
            return;
        }
    }

    if (alias_count < MAX_ALIASES) {
        if ((aliases[alias_count].alias = strdup(alias)) == NULL ||
            (aliases[alias_count].command = strdup(command)) == NULL) {
            perror("set_alias: Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        alias_count++;
    } else {
        perror("Maximum alias limit reached.\n");
    }
}

char *get_alias(const char *alias) {
    if (!alias) {
        errno = EFAULT;
        return NULL;
    }

    for (int i = 0; i < alias_count; i++) {
        if (strcmp(aliases[i].alias, alias) == 0) {
            return aliases[i].command;
        }
    }
    return NULL;
}

void unset_alias(const char *alias) {
    if (!alias) {
        errno = EFAULT;
        return;
    }

    for (int i = 0; i < alias_count; i++) {
        if (strcmp(aliases[i].alias, alias) == 0) {
            free(aliases[i].alias);
            free(aliases[i].command);
            for (int j = i; j < alias_count - 1; j++) {
                aliases[j] = aliases[j + 1];
            }
            alias_count--;
            return;
        }
    }
}

int builtin_alias() {
    for (int i = 0; i < alias_count; i++) {
        printf("alias %s='%s'\n", aliases[i].alias, aliases[i].command);
    }
    if (alias_count == 0) {
        printf("No aliases defined.\n");
    }
    return 0;
}

void expand_alias(char **args) {
    if (!args) {
        errno = EFAULT;
        return;
    }

    if (args[0] == NULL) return;

    char *alias_value = get_alias(args[0]);
    if (alias_value) {
        char *alias_copy = strdup(alias_value);
        int i = 0;
        char *token = strtok(alias_copy, " ");
        while (token && i < MAX_ARGS - 1) {
            args[i++] = strdup(token);
            token = strtok(NULL, " ");
        }
        args[i] = NULL;
        free(alias_copy);
    }
}

int is_builtin(char **args) {
    if (!args || !args[0]) {
        errno = EFAULT;
        return -1;
    }

    if (strcmp(args[0], "cd") == 0) return builtin_cd(args);
    if (strcmp(args[0], "pwd") == 0) return builtin_pwd();
    if (strcmp(args[0], "exit") == 0) return builtin_exit();
    if (strcmp(args[0], "echo") == 0) return builtin_echo(args);
    if (strcmp(args[0], "env") == 0) return builtin_env();
    if (strcmp(args[0], "aliases") == 0) return builtin_alias();

    return -1; // Pas de commande built-in
}
