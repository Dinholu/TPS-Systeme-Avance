#include "include/builtins.h"
#include "include/executor.h"
#include "include/history.h"
#include "include/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROMPT "vlad_alizee_shell> "

/**
 * Vérifie si une commande est un built-in et l'exécute si c'est le cas.
 * @param args Tableau d'arguments de la commande.
 * @return 1 si un built-in a été exécuté, 0 sinon.
 */
int is_builtin(char **args) {
  if (strcmp(args[0], "cd") == 0)
    return builtin_cd(args);
  if (strcmp(args[0], "pwd") == 0)
    return builtin_pwd();
  if (strcmp(args[0], "exit") == 0)
    return builtin_exit();
  if (strcmp(args[0], "echo") == 0)
    return builtin_echo(args);
  return 2; // Pas un built-in
}

/**
 * Fonction principale du shell.
 * Boucle principale qui lit les commandes de l'utilisateur,
 * les analyse, et les exécute.
 */
int main(int argc, char *argv[]) {
  // Mode batch
  if (argc <= 3 && strcmp(argv[1], "-c") == 0) {
    char **args = parse_command(argv[2]);
    if (args[0]) {
      if (!is_builtin(args)) {
        return 0;
      }
      execute_command(args);
      free(args);
    }
    return 0; // Sortir du shell après exécution de la commande
  }
  if (argc > 3 || (argc == 3 && strcmp(argv[1], "-c") != 0)) {
    fprintf(stderr, "Usage: %s [-c command]\n", argv[0]);
    return 1;
  }

  // Mode interactif
  char command[1024];
  while (1) {
    printf(PROMPT);

    if (!fgets(command, sizeof(command), stdin)) {
      break; // EOF
    }

    // Supprime le '\n' en fin de commande
    command[strcspn(command, "\n")] = 0;

    // Vérifie si la commande est vide
    if (strlen(command) == 0) {
      continue;
    }

    add_to_history(command);
    // Gestion des pipes : séparation de la commande en segments
    char **pipes = split_pipes(command);

    // Si la commande contient des pipes
    if (pipes[1] != NULL) {
      execute_piped_commands(pipes);
      for (int i = 0; pipes[i] != NULL; i++) {
        free(pipes[i]);
      }
      free(pipes);
    } else {
      // Si pas de pipes, analyse et exécution normale
      char **args = parse_command(command);

      // Exécute un built-in ou une commande système
      if (args[0]) {
        if (!is_builtin(args)) {
          continue;
        }
        execute_command(args);
      }

      // Libère la mémoire utilisée pour les arguments
      free(args);
    }
  }

  return 0;
}
