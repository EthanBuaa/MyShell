#ifndef BUILT_IN_H_
#define BUILT_IN_H_

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <signal.h>

typedef int (*built_in_handler_t)(int *, char *[]);
typedef built_in_handler_t built_in_handler_map_t[]; 

static const 
    char* const built_in_strs[] = {
        "cd",
        "exit",
        "kill",
        "help"
};

static const 
    built_in_handler_map_t built_in_handler_map = {    
        &__cd,                                      
        &__exit,  
        &__kill,
        &__help                                  
};

static inline int 
__cd(int *argc, char *argv[]) {
    int ret;
    if ((ret = chdir(argv[1]))< 0) {
        fprintf(stderr, "error: unable tp change to %s\n", argv[1]);
    }
    return ret;
}

static inline int 
__exit(int *argc, char *argv[]) {
    return -1;
}

static inline int 
__kill(int *argc, char *argv[]) {
    return kill(atoi(argv[1]), SIGKILL);
}

static inline int 
__help(int *argc, char *argv[]) {
    /* TODO: implement help MSG */
    printf(stdout, 
            "This is help message.\n"
    );
    return 0;
}

/** 
 * TODO: implement jobs 
 * with background module pre-implemented 
*/

/**
 * TODO: implement history
 * by include history lib
*/

static inline int get_built_in_index(char *argv0) {
    int cnt = sizeof(built_in_strs) / sizeof(built_in_strs[0]);
    int i;
    for (i = 0; i < cnt; i++) {
        if (strncmp(argv0, built_in_strs[i], sizeof(built_in_strs[i])) == 0) 
            return i;
    }
    return -1;
}
extern int handle_built_in(int* argc, char **argv);

#endif