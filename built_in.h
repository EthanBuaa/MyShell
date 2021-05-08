#ifndef BUILT_IN_H_
#define BUILT_IN_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <sys/types.h>
#include <signal.h>

#include "history.h"

typedef int (*built_in_handler_t)(int *, char *[]);
typedef built_in_handler_t built_in_handler_map_t[]; 

static const 
    char* const built_in_strs[] = {
        "cd",
        "exit",
        "kill",
        "help",
        "history"
};

static const 
    built_in_handler_map_t built_in_handler_map = {    
        &__cd,                                      
        &__exit,  
        &__kill,
        &__help,
        &__history                                  
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

extern int get_built_in_index(char *);
extern int handle_built_in(int ,int * , char *[]);

/** function called when shell is terminated 
 *  free all memory requested in shell module
*/
extern void _free_all_in_built_in();

#endif