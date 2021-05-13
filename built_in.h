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
        fprintf(stderr, "error: unable to change to %s\n", argv[1]);
    }
    return ret;
}

static inline int 
__exit(const int *argc, const char *const argv[]) {
    return EXIT_FAILURE;
}

static inline int 
__kill(const int *argc, const char *const argv[]) {
    char *end_ptr;
    long pid_l;

    /* 10 represents decimal (base = 10) */
    pid_l = strtol(argv[1], &end_ptr, 10);
    if (end_ptr == argv[1]) {
        fprintf(stderr, "error: failed in convert %s to number.\n",
                argv[1]);
        return -1;
    }
    
    return kill((pid_t)pid_l, SIGKILL);
}

static inline int 
__help(const int *argc, const char *const argv[]) {
    fprintf(stdout,             
" \
*************** Help ***************\n \
    history: history [-c] [n]       \n \
    kill: kill <pid>                \n \
************************************\n \
"
    );
    return 0;
}

/**
 * Note & Cautious: 
 * the sequence of element in following containers 
 * TODO: implement the map with hash map data structure 
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
 * TODO: implement jobs 
*/


extern int get_built_in_index(const char *);
extern int handle_built_in(const int , const int * , const char *const[]);

/** function called when shell is terminated 
 *  free all memory requested in shell module
*/
extern void _free_all_in_built_in();

#endif