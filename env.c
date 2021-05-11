#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <signal.h>
#include <errno.h>

#include "env.h"

int __printenv(const int *argc, const char *const argv[]) {
    int i;
    switch (*argc) {
    case 1:
        for (i = 0; i < env_vars->cnt; i++) 
            print_entry(i);
        break;
    case 2:
        for (i = 0; i < env_vars->cnt; i++) {
            if (strncmp(argv[1], env_vars->keys[i], 
                strlen(env_vars->keys[i])) == 0) {
                fprintf(stdout, "%s\n", env_vars->vals[i]);
                break; /* assumption: no duplicated key */
            }
        }
        break;
    default:
        return -1;
    }
    return 0;
}

int __setenv(const int *argc, const char *const argv[]) {
    
    return 0;
}

void init_env_vars() {
    if (!env_vars) {
        env_vars = _init_env_vars();
        if (!env_vars) {
            fprintf(stderr, "error: memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }
    }
    add_built_in_entries();
    return ;
}

void _free_all_in_env() {
    int i;
    for (i = 0; i < env_vars->cnt; i++) {
        free(env_vars->keys[i]);
        free(env_vars->vals[i]);
    }
    free(env_vars->vals);
    free(env_vars->keys);
    free(env_vars);
    return ;
}