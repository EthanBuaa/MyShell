#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>

#include "built_in.h"

static int __cd(int *argc, char **argv) {
    int ret = chdir(argv[1]);
    if (!ret) {
        fprintf(stderr, "error: unable tp change to %s\n", argv[1]);
        return 1;
    }
    return ret;
}

extern int is_built_in(char *argv0) {
    return strcmp(argv0, "exit") == 0||
            strcmp(argv0, "cd") == 0;
}

extern int handle_built_in(int* argc, char **argv) {
    if (strncmp(argv[0], "cd", strlen("cd")) == 0) {
        return built_in_handler.cd(argc, argv);
    } 
    if (strncmp(argv[0], 'exit', strlen("exit")) == 0) {
        return built_in_handler.exit(argc, argv);
    }

    /** if not parttern mathched 
     * raise error 
    */
    return -1;
}

