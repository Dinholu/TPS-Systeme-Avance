#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>
#include "ls_like.h"

void print_permissions(mode_t mode) {
    printf( (S_ISDIR(mode)) ? "d" : "-");
    printf( (mode & S_IRUSR) ? "r" : "-");
    printf( (mode & S_IWUSR) ? "w" : "-");
    printf( (mode & S_IXUSR) ? "x" : "-");
    printf( (mode & S_IRGRP) ? "r" : "-");
    printf( (mode & S_IWGRP) ? "w" : "-");
    printf( (mode & S_IXGRP) ? "x" : "-");
    printf( (mode & S_IROTH) ? "r" : "-");
    printf( (mode & S_IWOTH) ? "w" : "-");
    printf( (mode & S_IXOTH) ? "x" : "-");
}

void list_directory(const char* path) {
    DIR* dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    struct dirent* entry;
    struct stat file_stat;
    char filepath[1024];

    while ((entry = readdir(dir)) != NULL) {
        snprintf(filepath, sizeof(filepath), "%s/%s", path, entry->d_name);
        if (stat(filepath, &file_stat) == -1) {
            perror("Error getting file stats");
            continue;
        }

        print_permissions(file_stat.st_mode);
        printf(" ");

        struct passwd *pw = getpwuid(file_stat.st_uid);
        struct group *gr = getgrgid(file_stat.st_gid);

        printf("%s %s ", pw->pw_name, gr->gr_name);
        printf("%ld ", file_stat.st_size);

        char timebuf[80];
        struct tm *tm = localtime(&file_stat.st_mtime);
        strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm);
        printf("%s ", timebuf);

        printf("%s\n", entry->d_name);
    }

    closedir(dir);
}
