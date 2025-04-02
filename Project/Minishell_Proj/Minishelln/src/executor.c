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
#include <errno.h>

//test
// Ouvre un fichier pour redirection de sortie
int open_output_redirection(const char *filename, int append)
{
    int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
    int fd = open(filename, flags, 0644);
    if (fd == -1)
    {
        perror("open");
    }
    return fd;
}

// Ouvre un fichier pour redirection d'entrée
int open_input_redirection(const char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("open");
    }
    return fd;
}

// Gère les redirections des fichiers
void handle_redirections(char **args)
{
    int fd_in = -1, fd_out = -1;

    for (int i = 0; args[i]; i++)
    {
        if (strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0)
        {
            fd_out = open_output_redirection(args[i + 1], strcmp(args[i], ">>") == 0);
            args[i] = NULL;
        }
        else if (strcmp(args[i], "<") == 0)
        {
            fd_in = open_input_redirection(args[i + 1]);
            args[i] = NULL;
        }
    }

    if (fd_out != -1)
    {
        dup2(fd_out, STDOUT_FILENO);
        close(fd_out);
    }
    if (fd_in != -1)
    {
        dup2(fd_in, STDIN_FILENO);
        close(fd_in);
    }
}

// Exécute une commande unique
void execute_command(char **args)
{
    if (!args || args[0] == NULL)
        return;

    pid_t pid = fork();
    if (pid == 0)
    { // Processus enfant
        handle_redirections(args);
        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    { // Processus parent
        int status;
        waitpid(pid, &status, 0);
    }
    else
    {
        perror("fork");
    }
}

// Exécution d'une commande avec pipe
void execute_piped_commands(char *commands[])
{
    int pipefd[2], fd_in = 0;
    pid_t pid;

    for (int i = 0; commands[i] != NULL; i++)
    {
        pipe(pipefd);
        pid = fork();

        if (pid == 0)
        { // Processus enfant
            dup2(fd_in, STDIN_FILENO);
            if (commands[i + 1] != NULL)
            {
                dup2(pipefd[1], STDOUT_FILENO);
            }
            close(pipefd[0]);
            close(pipefd[1]);

            char **args = parse_command(commands[i]);
            handle_redirections(args);
            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        else
        {
            waitpid(pid, NULL, 0);
            close(pipefd[1]);
            fd_in = pipefd[0]; // Récupère la sortie pour la prochaine commande
        }
    }
}

// Vérifie si une commande se termine par `&`
int check_background_execution(char *command)
{
    size_t len = strlen(command);
    while (len > 0 && isspace(command[len - 1]))
    {
        command[--len] = '\0';
    }
    if (len > 0 && command[len - 1] == '&')
    {
        command[len - 1] = '\0';
        return 1;
    }
    return 0;
}

// Exécute une commande logique avec `&&` ou `||`
void execute_commands_with_logic(const char *input)
{
    CommandNode commands[MAX_COMMANDS];
    int command_count = parse_control_operators(input, commands);
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
            continue;

        if (strchr(commands[i].command, '|'))
        {
            char *cmds[MAX_COMMANDS] = {NULL};
            int count = 0;
            char *token = strtok(commands[i].command, "|");
            while (token)
            {
                cmds[count++] = token;
                token = strtok(NULL, "|");
            }
            execute_piped_commands(cmds);
        }
        else
        {
            execute_command(args);
        }

        free(args);
        free(commands[i].command);
    }
}
