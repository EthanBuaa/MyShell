#ifndef BUILT_IN_H_
#define BUILT_IN_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <sys/types.h>
#include <signal.h>

#include "history.h"

typedef int (*built_in_handler_t)(const int *, const char *const[]);
typedef built_in_handler_t built_in_handler_map_t[]; 

static inline int 
__cd(const int *argc, const char *const argv[]) {
    int ret;
    if ((ret = chdir(argv[1])) < 0) {
        fprintf(stderr, "error: unable tp change to %s\n", argv[1]);
    }
    return ret;
}

static inline int 
__exit(const int *argc, const char *const argv[]) {
    return -1;
}

static inline int 
__kill(const int *argc, const char *const argv[]) {
    return kill(atoi(argv[1]), SIGKILL);
}

static inline int 
__help(const int *argc, const char *const argv[]) {
    /* TODO: implement help MSG */
    fprintf(stdout, 
            "This is help message.\n"
    );
    return 0;
}

/**
 * Note & Cautious: 
 * the sequence of element in following containers 
*/
static const 
    char* const built_in_strs[] = {
        "cd",
        "history",
        "exit",
        "kill",
        "help"
};

static const 
    built_in_handler_map_t built_in_handler_map = {    
        &__cd,  
        &__history,                                    
        &__exit,  
        &__kill,
        &__help                               
};

/** 
 * TODO: implement jobs as well as background
 * with background module pre-implemented 
*/


extern int get_built_in_index(const char *);
extern int handle_built_in(const int , const int * , const char *const[]);

/** function called when shell is terminated 
 *  free all memory requested in shell module
*/
extern void _free_all_in_built_in();

#endif