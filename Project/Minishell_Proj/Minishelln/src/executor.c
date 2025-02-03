#include "include/builtins.h"
#include "include/parser.h"
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


void execute_command(char **args) {
  expand_alias(args);
  int fd_out = -1;
  int fd_in = -1;
  int background = is_background_command(args);

  for (int i = 0; args[i]; i++) {
    if (strcmp(args[i], ">") == 0) {
      fd_out = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (fd_out == -1) {
        perror("open");
        exit(EXIT_FAILURE);
      }
      args[i] = NULL;
      break;
    } else if (strcmp(args[i], ">>") == 0) {
      fd_out = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
      if (fd_out == -1) {
        perror("open");
        exit(EXIT_FAILURE);
      }
      args[i] = NULL;
      break;
    }
    if (strcmp(args[i], "<") == 0) {
      fd_in = open(args[i + 1], O_RDONLY);
      if (fd_in == -1) {
        perror("open");
        exit(EXIT_FAILURE);
      }
      args[i] = NULL;
      break;
    }
  }

  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    // Processus fils : Exécution de la commande
    if (fd_out != -1) {
      dup2(fd_out, STDOUT_FILENO);
      close(fd_out);
    }
    if (fd_in != -1) {
      dup2(fd_in, STDIN_FILENO);
      close(fd_in);
    }
    if (execvp(args[0], args) == -1) {
      perror("execvp");
      exit(EXIT_FAILURE);
    }
  } else {
    if (!background) {
      // Processus parent : Attend que le fils termine
      int status;
      if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
      }
    } else {
      printf("[Background process started] PID: %d\n", pid);
    }
  }
}

int contains_redirection(char **args) {
  for (int i = 0; args[i]; i++) {
    if (strcmp(args[i], ">") == 0) {
      return 1;
    }
    if (strcmp(args[i], ">>") == 0) {
      return 1;
    }
    if (strcmp(args[i], "<") == 0) {
      return 1;
    }
  }
  return 0;
}

int execute_logic_command(const char *command) {
  pid_t pid = fork();
  int status;

  if (pid == 0) {
    // Processus fils
    execlp("/bin/sh", "sh", "-c", command, (char *)NULL);
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    perror("fork");
    return -1;
  } else {
    // Processus parent
    waitpid(pid, &status, 0);
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
  }
}

void execute_piped_commands(char **commands) {
  int pipefd[2];
  pid_t pid;
  int fd_in = 0;

  for (int i = 0; commands[i] != NULL; i++) {
    if (pipe(pipefd) == -1) {
      perror("pipe");
      exit(EXIT_FAILURE);
    }

    if ((pid = fork()) == -1) {
      perror("fork");
      exit(EXIT_FAILURE);
    }

    if (pid == 0) {
      // Processus fils
      char **args = parse_command(commands[i]);
      // Gérer la redirection de la sortie si présente
      // Exemple commande : ls | grep "Makefile" > test.txt
      int fd_out = -1;
      for (int j = 0; args[j]; j++) {
        if (strcmp(args[j], ">") == 0) {
          fd_out = open(args[j + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
          if (fd_out == -1) {
            perror("open");
            exit(EXIT_FAILURE);
          }
          args[j] =
              NULL; // Supprimer l'opérateur de redirection et les arguments
          break;
        }
      }

      // Rédirection des entrées et sorties
      dup2(fd_in, STDIN_FILENO); // Input pour le premier processus
      if (commands[i + 1] != NULL) {
        dup2(pipefd[1], STDOUT_FILENO); // Output pour les commandes suivantes
      } else if (fd_out != -1) {
        dup2(fd_out, STDOUT_FILENO); // Output vers un fichier si présence de '>' dans la commande
      }

      close(pipefd[0]);
      close(pipefd[1]);

      // Execution de la commande
      execvp(args[0], args);
      perror("execvp");
      exit(EXIT_FAILURE);
    } else {
      // Processus parent attend la fin du fils
      waitpid(pid, NULL, 0);
      close(pipefd[1]);
      fd_in = pipefd[0];
    }
  }
  close(fd_in);
}

int check_background_execution(char *command) {
  size_t len = strlen(command);
  // Supprimer les espaces de fin et vérifier si la commande se termine par '&'
  while (len > 0 && isspace(command[len - 1])) {
    command[--len] = '\0';
  }

  if (len > 0 && command[len - 1] == '&') {
    command[len - 1] = '\0';
    return 1;
  }
  return 0;
}

void execute_commands_with_logic(const char *input) {
  CommandNode commands[MAX_COMMANDS];
  int command_count = parse_control_operators(input, commands);
  int success = 1; // Résultat de la commande précédente

  for (int i = 0; i < command_count; i++) {
    if (i > 0) {
      // Sauter l'exécution en fonction du résultat de la commande précédente et de l'opérateur de contrôle
      if ((commands[i - 1].type == CMD_AND && !success) ||
          (commands[i - 1].type == CMD_OR && success)) {
        continue;
      }
    }

    // Analyse et gère la redirection de la sortie pour la commande en cours
    char **args = parse_command(commands[i].command);
    if (contains_redirection(args)) {
      execute_command(args);
      success = 1;
      free(args);
      free(commands[i].command);
      continue;
    }
    if (args[0] == NULL) {
      continue;
    }

    // Vérifie si la commande est un built-in
    expand_env_variables(args);
    int builtin_status = is_builtin(args);
    if (builtin_status == 0) {
      free(args);
      free(commands[i].command);
      success = 1;
      continue;
    }

    // Exécute la commande externe si ce n'est pas un built-in
    success = execute_logic_command(commands[i].command) == 0;

    free(args);
    free(commands[i].command);
  }
}