#include "include/typedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Commandes intégrées
int builtin_cd(char **args) {
  if (!args[1]) {
    fprintf(stderr, "cd: missing argument\n");
    return -1;
  }
  if (chdir(args[1]) != 0) {
    perror("cd");
    return -1;
  }
  return 0;
}

int builtin_pwd() {
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    perror("pwd");
    return -1;
  }
  printf("%s\n", cwd);
  return 0;
}

int builtin_echo(char **args) {
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
  printf("Exiting shell... Bye\n");
  exit(0);
}

// Variables d'environnement
EnvVar env_vars[MAX_ENV_VARS];
int env_count = 0;

int find_env_var(const char *name) {
  for (int i = 0; i < env_count; i++) {
    if (strcmp(env_vars[i].name, name) == 0) {
      return i;
    }
  }
  return -1;
}

void set_env_var(const char *name, const char *value) {
  int index = find_env_var(name);
  if (index >= 0) {
    free(env_vars[index].value);
    env_vars[index].value = strdup(value);
  } else {
    env_vars[env_count].name = strdup(name);
    env_vars[env_count].value = strdup(value);
    env_count++;
  }
}

char *get_env_var(const char *name) {
  int index = find_env_var(name);
  return index >= 0 ? env_vars[index].value : NULL;
}

void unset_env_var(const char *name) {
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
  for (int i = 0; args[i]; i++) {
    if (args[i][0] == '$' && strlen(args[i]) > 1) {
      char *var_name = args[i] + 1;
      char *value = get_env_var(var_name);
      if (value) {
        free(args[i]);
        args[i] = strdup(value);
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
  if (strcmp(alias, command) == 0) {
    fprintf(stderr, "alias: cannot create recursive alias\n");
    return;
  }

  for (int i = 0; i < alias_count; i++) {
    if (strcmp(aliases[i].alias, alias) == 0) {
      free(aliases[i].command);
      aliases[i].command = strdup(command);
      return;
    }
  }

  if (alias_count < MAX_ALIASES) {
    aliases[alias_count].alias = strdup(alias);
    aliases[alias_count].command = strdup(command);
    alias_count++;
  } else {
    fprintf(stderr, "Maximum alias limit reached.\n");
  }
}

char *get_alias(const char *alias) {
  for (int i = 0; i < alias_count; i++) {
    if (strcmp(aliases[i].alias, alias) == 0) {
      return aliases[i].command;
    }
  }
  return NULL;
}

void unset_alias(const char *alias) {
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
  if (args[0] == NULL) return;

  char *alias_value = get_alias(args[0]);
  if (alias_value) {
    // Décomposer la valeur de l'alias en tokens et remplacer les args actuels
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
  if (strcmp(args[0], "cd") == 0)
    return builtin_cd(args);
  if (strcmp(args[0], "pwd") == 0)
    return builtin_pwd();
  if (strcmp(args[0], "exit") == 0)
    return builtin_exit();
  if (strcmp(args[0], "echo") == 0)
    return builtin_echo(args);
  if (strcmp(args[0], "env") == 0)
    return builtin_env();
  if (strcmp(args[0], "aliases") == 0)
    return builtin_alias();
  return -1; // Pas une commande built-in
}