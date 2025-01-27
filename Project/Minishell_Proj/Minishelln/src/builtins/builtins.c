#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
  static int call_count = 0;
  call_count++;
  fprintf(stderr, "[DEBUG] builtin_pwd called: %d times\n", call_count);

  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    perror("pwd");
    return -1;
  }
  printf("%s\n", cwd);
  return 0;
}

int builtin_echo(char **args) {
  static int call_count = 0;
  call_count++;
  fprintf(stderr, "[DEBUG] builtin_echo called: %d times\n", call_count);

  for (int i = 1; args[i]; i++) {
    printf("%s", args[i]);
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
