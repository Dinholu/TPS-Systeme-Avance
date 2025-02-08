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

// Gestions des redirections
int open_output_redirection(const char *filename, int append) {
    if (!filename) {
        errno = EFAULT;
        return -1;
    }

    int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
    int fd = open(filename, flags, 0644);
    if (fd == -1) {
        return -1;
    }
    return fd;
}

int open_input_redirection(const char *filename) {
    if (!filename) {
        errno = EFAULT;
        return -1;
    }
    
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        return -1;
    }
    return fd;
}

void handle_redirections(char **args, int *fd_in, int *fd_out) {
    if (!args || !fd_in || !fd_out) {
        errno = EFAULT;
        return;
    }
    
    for (int i = 0; args[i]; i++) {
        if (strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0) {
            *fd_out = open_output_redirection(args[i + 1], strcmp(args[i], ">>") == 0);
            if (*fd_out == -1) return;
            args[i] = NULL;
            break;
        } else if (strcmp(args[i], "<") == 0) {
            *fd_in = open_input_redirection(args[i + 1]);
            if (*fd_in == -1) return;
            args[i] = NULL;
            break;
        }
    }
}

// Exécution d'une commande unique avec redirection et execution en arrière-plan
void execute_command(char **args) {
    if (!args) {
        errno = EFAULT;
        return;
    }

    expand_alias(args);

    int fd_out = -1, fd_in = -1;
    int background = is_background_command(args);

    handle_redirections(args, &fd_in, &fd_out);

    pid_t pid = fork();
    if (pid < 0) {
        return;
    }

    if (pid == 0) {  // Enfant
        if (fd_out != -1) {
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }
        if (fd_in != -1) {
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }
        execvp(args[0], args);
        exit(EXIT_FAILURE);
    } else {  // Parent
        if (!background) {
            int status;
            waitpid(pid, &status, 0);
        } else {
            printf("[Background process started] PID: %d\n", pid);
        }
    }
}

// Vérifie si une commande contient des opérateurs de redirection
int contains_redirection(char **args) {
    if (!args) {
        errno = EFAULT;
        return -1;
    }

    for (int i = 0; args[i]; i++) {
        if (strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0 || strcmp(args[i], "<") == 0) {
            return 1;
        }
    }
    return 0;
}

// Exécution d'une commande logique (e.g., &&, ||)
int execute_logic_command(const char *command) {
    if (!command) {
        errno = EFAULT;
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        return -1;
    }
    if (pid == 0) {
        execlp("/bin/sh", "sh", "-c", command, (char *)NULL);
        exit(EXIT_FAILURE);
    }
    
    int status;
    waitpid(pid, &status, 0);
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}

// Exécution de commandes séparées par des pipes
void execute_piped_commands(char **commands) {
    if (!commands) {
        errno = EFAULT;
        return;
    }

    int pipefd[2];
    pid_t pid;
    int fd_in = 0;

    for (int i = 0; commands[i] != NULL; i++) {
        if (pipe(pipefd) == -1) {
            return;
        }

        pid = fork();
        if (pid == -1) {
            return;
        }

        if (pid == 0) {
            char **args = parse_command(commands[i]);
            if (!args) {
                errno = ENOMEM;
                exit(EXIT_FAILURE);
            }
            int fd_out = -1;
            handle_redirections(args, &fd_in, &fd_out);

            dup2(fd_in, STDIN_FILENO);
            if (commands[i + 1] != NULL) {
                dup2(pipefd[1], STDOUT_FILENO);
            } else if (fd_out != -1) {
                dup2(fd_out, STDOUT_FILENO);
            }

            close(pipefd[0]);
            close(pipefd[1]);

            execvp(args[0], args);
            exit(EXIT_FAILURE);
        } else {
            waitpid(pid, NULL, 0);
            close(pipefd[1]);
            fd_in = pipefd[0];
        }
    }
    close(fd_in);
}

// Vérifie si une commande nécessite une exécution en arrière plan
int check_background_execution(char *command) {
    if (!command) {
        errno = EFAULT;
        return -1;
    }

    size_t len = strlen(command);
    while (len > 0 && isspace(command[len - 1])) {
        command[--len] = '\0';
    }
    if (len > 0 && command[len - 1] == '&') {
        command[len - 1] = '\0';
        return 1;
    }
    return 0;
}

// Exécution des commandes avec les opérateurs logiques (&&, ||)
void execute_commands_with_logic(const char *input) {
    if (!input) {
        errno = EFAULT;
        return;
    }

    CommandNode commands[MAX_COMMANDS];
    int command_count = parse_control_operators(input, commands);
    int success = 1;

    for (int i = 0; i < command_count; i++) {
        if (i > 0) {
            if ((commands[i - 1].type == CMD_AND && !success) ||
                (commands[i - 1].type == CMD_OR && success)) {
                continue;
            }
        }

        char **args = parse_command(commands[i].command);
        if (!args) {
            errno = ENOMEM;
            return;
        }

        if (contains_redirection(args)) {
            execute_command(args);
            success = 1;
        } else if (args[0] != NULL) {
            expand_env_variables(args);
            int builtin_status = is_builtin(args);
            if (builtin_status == 0) {
                success = 1;
            } else {
                success = execute_logic_command(commands[i].command) == 0;
            }
        }

        free(args);
        free(commands[i].command);
    }
}
