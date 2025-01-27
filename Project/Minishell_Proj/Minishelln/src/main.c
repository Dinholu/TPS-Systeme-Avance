#include "src/builtins/builtins.h"
#include "src/executor/executor.h"
#include "src/parser/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROMPT "my_shell> "

int is_builtin(char **args) {
  if (strcmp(args[0], "cd") == 0)
    return builtin_cd(args);
  if (strcmp(args[0], "pwd") == 0)
    return builtin_pwd();
  if (strcmp(args[0], "exit") == 0)
    return builtin_exit();
  if (strcmp(args[0], "echo") == 0)
    return builtin_echo(args);
  return 0; // Pas un built-in
}

int main() {
  char command[1024];
  while (1) {
    printf(PROMPT);
    if (!fgets(command, sizeof(command), stdin)) {
      break; // EOF
    }
    command[strcspn(command, "\n")] = 0; // Enlève le '\n'

    // Parse et exécute
    char **args = parse_command(command);
    if (args[0] && !is_builtin(args)) {
      execute_command(args);
    }
    free(args);
  }
  return 0;
}
