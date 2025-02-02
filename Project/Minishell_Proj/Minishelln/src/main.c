#include "../include/builtins.h"
#include "../include/executor.h"
#include "../include/history.h"
#include "../include/parser.h"
#include "../include/typedef.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>

// #define PROMPT "vlad_alizee_shell> "
#define PROMPT "shell> "

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
  if (strcmp(args[0], "env") == 0)
    return builtin_env();
  if (strcmp(args[0], "aliases") == 0)
    return builtin_alias();
  return -1; // Pas une commande built-in
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

/**
 * @brief Exécute une série de commandes séparées par des opérateurs logiques.
 * @param input Commandes séparées par des opérateurs logiques.
 */
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
    expand_variables(args);
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

int check_background_execution(char *command) {
  size_t len = strlen(command);
  // Trim trailing spaces and check if the last non-space character is '&'
  while (len > 0 && isspace(command[len - 1])) {
    command[--len] = '\0';
  }

  if (len > 0 && command[len - 1] == '&') {
    command[len - 1] = '\0'; // Remove the '&'
    return 1;
  }
  return 0;
}

/**
 * @brief Fonction principale du shell.
 * Boucle principale qui lit les commandes de l'utilisateur,
 * les analyse, et les exécute.
 * @param argc Nombre d'arguments passés au programme.
 * @param argv Tableau d'arguments passés au programme.
 */
int main(int argc, char *argv[]) {
  // Mode batch
  if (argc == 3 && argv[1] && strcmp(argv[1], "-c") == 0) {
    char **args = parse_command(argv[2]);
    if (args[0]) {
      execute_commands_with_logic(argv[2]);
      return 0;

      free(args);
    }
    return 0; // Sortir du shell après exécution de la commande
  }

  if (argc > 3 || (argc == 3 && strcmp(argv[1], "-c") != 0)) {
    fprintf(stderr, "Usage: %s [-c command]\n", argv[0]);
    return -1;
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

    int is_background = check_background_execution(command);

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
    char **args = parse_command(command);
    if (args[0]) {
      char *alias_expansion = get_alias(args[0]);
      if (alias_expansion) {
        free(args[0]);
        args[0] = strdup(alias_expansion);
      }
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

      execute_commands_with_logic(command);

      // Libère la mémoire utilisée pour les arguments
      free(args);
    }

    if (is_background) {
      pid_t pid = fork();
      if (pid == 0) {
        // Child process for background execution
        execute_commands_with_logic(command);
        exit(0);
      } else if (pid > 0) {
        printf("[Background process started] PID: %d\n", pid);
      } else {
        perror("fork");
      }
    }

  }

  return 0;
}