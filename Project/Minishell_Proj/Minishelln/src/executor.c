#include "src/parser/parser.h"
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
    // Processus père : Attend que le fils termine
    int status;
    if (waitpid(pid, &status, 0) == -1) {
      perror("waitpid");
    }
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
      dup2(fd_in, STDIN_FILENO); // Redirige l'entrée
      if (commands[i + 1] != NULL) {
        dup2(pipefd[1], STDIN_FILENO); // Redirige la sortie
      }
      close(pipefd[0]);
      close(pipefd[1]);

      // Parse la commande pour obtenir les arguments
      char **args = parse_command(commands[i]);

      execvp(args[0], args);
      if (execvp(args[0], args) == -1) {
        perror("execvp");
        exit(EXIT_FAILURE);
      }
    } else {
      // Processus père
      waitpid(pid, NULL, 0); // Attend le fils
      close(pipefd[1]);
      fd_in =
          pipefd[0]; // La sortie devient l'entrée pour la prochaine commande
    }
  }
  close(fd_in);
}
