#include "include/builtins.h"
#include "include/executor.h"
#include "include/history.h"
#include "include/parser.h"
#include "include/typedef.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define PROMPT "shell> "

extern EnvVar env_vars[MAX_ENV_VARS];
extern Alias aliases[MAX_ALIASES];
extern int env_count;
extern int alias_count;

/**
 * @brief Fonction principale du shell.
 * Boucle principale qui lit les commandes de l'utilisateur,
 * les analyse, et les exécute.
 * @param argc Nombre d'arguments passés au programme.
 * @param argv Tableau d'arguments passés au programme.
 */
int main(int argc, char *argv[])
{
    // Mode batch
    if (argc == 3 && strcmp(argv[1], "-c") == 0)
    {
        execute_commands_with_logic(argv[2]);
        return 0;
    }

    if (argc > 3 || (argc == 3 && strcmp(argv[1], "-c") != 0))
    {
        fprintf(stderr, "Usage: %s [-c command]\n", argv[0]);
        return -1;
    }

    // Mode interactif
    char command[1024];
    while (1)
    {
        printf(PROMPT);
        if (!fgets(command, sizeof(command), stdin))
        {
            printf("\nExiting shell...\n");
            break; // EOF
        }

        // Supprime le '\n' en fin de commande
        command[strcspn(command, "\n")] = 0;

        // Vérifie si la commande est vide
        if (strlen(command) == 0)
            continue;
        add_to_history(command);

        if (strncmp(command, "alias ", 6) == 0)
        {
            char *alias_def = command + 6;
            char *equals_sign = strchr(alias_def, '=');
            if (equals_sign)
            {
                *equals_sign = '\0';
                char *alias_name = alias_def;
                char *alias_value = equals_sign + 1;

                if (alias_value[0] == '"' && alias_value[strlen(alias_value) - 1] == '"')
                {
                    alias_value[strlen(alias_value) - 1] = '\0';
                    alias_value++;
                }
                set_alias(alias_name, alias_value);
            }
            else
            {
                fprintf(stderr, "alias: invalid syntax\n");
            }
            continue;
        }

        if (strncmp(command, "unalias ", 8) == 0)
        {
            unset_alias(command + 8);
            continue;
        }

        if (strchr(command, '=') && !strchr(command, ' '))
        {
            char *name = strtok(command, "=");
            char *value = strtok(NULL, "");
            if (name && value)
            {
                set_env_var(name, value);
            }
            continue;
        }

        // Vérifie si la commande est une suppression de variable
        if (strncmp(command, "unset ", 6) == 0)
        {
            unset_env_var(command + 6);
            continue;
        }

        int is_background = check_background_execution(command);
        CommandNode commands[MAX_COMMANDS];
        int command_count = parse_control_operators(command, commands);
        int success = 1;

        for (int i = 0; i < command_count; i++)
        {
            if (i > 0)
            {
                if ((commands[i - 1].type == CMD_AND && !success) ||
                    (commands[i - 1].type == CMD_OR && success))
                {
                    continue;
                }
            }

            char **args = parse_command(commands[i].command);
            if (!args || args[0] == NULL)
            {
                free(args);
                continue;
            }

            char *alias_expansion = get_alias(args[0]);
            if (alias_expansion)
            {
                free(args[0]);
                args[0] = strdup(alias_expansion);

                char **expanded_args = parse_command(alias_expansion);
                if (expanded_args)
                {
                    free(args);
                    args = expanded_args;
                }
            }

            expand_env_variables(args);

            if (is_builtin(args) == 0)
            {
                free(args);
                free(commands[i].command);
                success = 1;
                continue;
            }

            if (execvp(args[0], args) == -1)
            {
                perror("execvp");
            }
            success = 0;
            free(args);
            free(commands[i].command);
        }
    }
    return 0;
}
