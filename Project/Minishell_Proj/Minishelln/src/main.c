#include "include/builtins.h"
#include "include/executor.h"
#include "include/history.h"
#include "include/parser.h"
#include "include/typedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROMPT "vlad_alizee_shell> "

/**
 * @brief Vérifie si une commande est un built-in et l'exécute si c'est le cas.
 * @param args Tableau d'arguments de la commande.
 * @return -1 si un built-in a été exécuté, 0 sinon.
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
  return -1; // Pas une commande built-in
}

/**
 * @brief Fonction principale du shell.
 * Boucle principale qui lit les commandes de l'utilisateur,
 * les analyse, et les exécute.
 * @param args Tableau d'arguments de la commande.
 */
int main(int argc, char *argv[]) {
  // Mode batch
  if (argc == 3 && argv[1] && strcmp(argv[1], "-c") == 0) {
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

    // Vérifie si la commande est un alias
    if (strncmp(command, "alias ", 6) == 0) {
      char *alias_def = command + 6;
      char *equals_sign = strchr(alias_def, '=');

      if (equals_sign) {
        *equals_sign = '\0';
        char *alias_name = alias_def;
        char *alias_value = equals_sign + 1;

        if (alias_value[0] == '"' &&
            alias_value[strlen(alias_value) - 1] == '"') {
          alias_value[strlen(alias_value) - 1] = '\0';
          alias_value++;
        }
        set_alias(alias_name, alias_value);
      } else {
        fprintf(stderr, "alias: invalid syntax\n");
      }
      continue;
    }

    if (strncmp(command, "unalias ", 8) == 0) {
      unset_alias(command + 8);
      continue;
    }

    // Appel de l'alias si la commande est un alias
    char *alias_expansion = get_alias(command);
    if (alias_expansion) {
      printf("Executing alias: %s -> %s\n", command, alias_expansion);
      strcpy(command, alias_expansion);
    }

    // Vérifie si la commande est une assignation de variable
    char *equal_sign = strchr(command, '=');
    if (equal_sign && (equal_sign != command) && strchr(command, ' ') == NULL) {
      *equal_sign = '\0';
      char *name = command;
      char *value = equal_sign + 1;

      // Gestion des variables d'environnement entre guillemets
      if (value[0] == '"' && value[strlen(value) - 1] == '"') {
        value[strlen(value) - 1] = '\0';
        value++;
      }

      set_env_var(name, value);
      continue;
    }

    // Vérifie si la commande est une suppression de variable
    if (strncmp(command, "unset ", 6) == 0) {
      unset_env_var(command + 6);
      continue;
    }

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
      if (!args) {
        fprintf(stderr, "Error: Command parsing failed.\n");
        continue;
      }

      expand_variables(args);

      if (strcmp(args[0], "env") == 0) {
        builtin_env();
        free(args);
        continue;
      }

      if (strcmp(args[0], "aliases") == 0) {
        builtin_alias();
        free(args);
        continue;
      }

      // Exécute un built-in ou une commande système
      if (args[0]) {
        int builtin_status = is_builtin(args);
        if (builtin_status == 0) { // Commande built-in exécutée
          free(args);
          continue;
        } else if (builtin_status == -1) { // Pas un built-in
          execute_command(args);
        }
      }

      // Libère la mémoire utilisée pour les arguments
      free(args);
    }
  }

  return 0;
}
