#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int builtin_cd(char **args) {
    if (!args[1]) {
        fprintf(stderr, "cd: missing argument\n");
        return -1;
    }
    if (chdir(args[1]) != 0) {
        perror("cd");
        return -1;
    }
    return 0;
}

int builtin_pwd() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("pwd");
        return -1;
    }
    printf("%s\n", cwd);
    return 0;
}

int builtin_exit() {
    printf("Exiting shell...\n");
    exit(0);
}

int builtin_echo(char **args) {
    for (int i = 1; args[i]; i++) {
        printf("%s ", args[i]);
    }
    printf("\n");
    return 0;
}
