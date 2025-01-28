#include "include/typedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

void expand_variables(char **args) {
  for (int i = 0; args[i]; i++) {
    if (args[i][0] == '$') {
      char *var_name = args[i] + 1;
      char *value = get_env_var(var_name);
      if (value) {
        free(args[i]);
        args[i] = strdup(value);
      } else {
        args[i] = strdup("");
      }
    }
  }
}

int builtin_env() {
  for (int i = 0; i < env_count; i++) {
    printf("%s=%s\n", env_vars[i].name, env_vars[i].value);
  }
  return 0;
}

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
