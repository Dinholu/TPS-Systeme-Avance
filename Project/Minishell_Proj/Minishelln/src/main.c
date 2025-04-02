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
#include <sys/types.h>
#include <sys/wait.h>

#define PROMPT "shell> "

extern EnvVar env_vars[MAX_ENV_VARS];
extern Alias aliases[MAX_ALIASES];
extern int env_count;
extern int alias_count;

int main(int argc, char *argv[])
{
    // Mode batch avec `-c`
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
        if (strlen(command) == 0)
            continue;

        // Ajoute la commande à l'historique
        add_to_history(command);

        // Vérifie si c'est une commande d'affectation de variable
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

        // Vérifie si c'est une suppression de variable
        if (strncmp(command, "unset ", 6) == 0)
        {
            unset_env_var(command + 6);
            continue;
        }

        // Vérifie si c'est une commande interne (built-in)
        char **args = parse_command(command);
        expand_env_variables(args); // Expansion des variables d’environnement

        if (is_builtin(args) == 0)
        {
            free(args);
            continue;
        }

        // Vérifie si c'est une commande en arrière-plan
        int is_background = check_background_execution(command);

        // Vérifie si la commande contient `|`, `>`, `>>`, `<`
        if (strchr(command, '|'))
        {
            char *cmds[MAX_COMMANDS] = {NULL};
            int count = 0;
            char *token = strtok(command, "|");
            while (token)
            {
                cmds[count++] = token;
                token = strtok(NULL, "|");
            }
            execute_piped_commands(cmds);
        }
        else if (strchr(command, '>') || strchr(command, '<'))
        {
            execute_command(args);
        }
        else
        {
            execute_commands_with_logic(command);
        }

        free(args);
    }
    return 0;
}
