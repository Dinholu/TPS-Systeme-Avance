#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_copy.h"
#include "reverse_print.h"
#include "ls_like.h"
#include "buffered_io.h" 

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <operation> <args...>\n", argv[0]);
        fprintf(stderr, "Operations: copy, reverse, ls, io\n");
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "copy") == 0 && argc == 4) {
        copy_file(argv[2], argv[3]);
    } else if (strcmp(argv[1], "reverse") == 0 && argc == 3) {
        print_reverse(argv[2]);
    } else if (strcmp(argv[1], "ls") == 0 && argc == 3) {
        list_directory(argv[2]);
    } else if (strcmp(argv[1], "io") == 0 && argc == 4) {

        MY_FILE* file = my_open(argv[2], "r");
        if (!file) {
            fprintf(stderr, "Error opening file %s\n", argv[2]);
            return EXIT_FAILURE;
        }

        MY_FILE* outfile = my_open(argv[3], "w");
        if (!outfile) {
            my_close(file);
            fprintf(stderr, "Error opening file %s\n", argv[3]);
            return EXIT_FAILURE;
        }

        int ch;
        while ((ch = my_getc(file)) != EOF) {
            my_putc(ch, outfile);
        }

        my_close(file);
        my_close(outfile);
    } else {
        fprintf(stderr, "Invalid operation or arguments.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
