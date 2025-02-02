#include "include/parser.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void execute_command(char **args) {
  int fd_out = -1;

  for (int i = 0; args[i]; i++) {
    if (strcmp(args[i], ">") == 0) {
      fd_out = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (fd_out == -1) {
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

    if (execvp(args[0], args) == -1) {
      perror("execvp");
      exit(EXIT_FAILURE);
    }
  } else {
    // Processus parent : Attend que le fils termine
    int status;
    if (waitpid(pid, &status, 0) == -1) {
      perror("waitpid");
    }
  }
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
          args[j] = NULL;  // Supprimer l'opérateur de redirection et les arguments
          break;
        }
      }

      // Rédirection des entrées et sorties
      dup2(fd_in, STDIN_FILENO);  // Input pour le premier processus
      if (commands[i + 1] != NULL) {
        dup2(pipefd[1], STDOUT_FILENO); // Output pour les commandes suivantes
      } else if (fd_out != -1) {
        dup2(fd_out, STDOUT_FILENO);  // Output vers un fichier si présence de '>' dans la commande
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
