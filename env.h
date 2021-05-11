/**
 * This module implements the envrionment variables
 * as well as built-ins: printenv and setenv 
*/

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifndef ENV_H_
#define ENV_H_

/* base capacity */
#define CAP_BASE (64)

/**
 * the env_vars stored in following data structure
 * TODO: implement the hash map data structure 
 * similar to linear table
 * search with complexity of O(n) => bad performance
 * to store env variables as well as 
 * built_in_handler_t in built_in.h
*/
struct env {
    int cnt;
    int cap;
    const char **keys;
    const char **vals;
};

typedef struct env env_map_t; 

/** function called when shell is terminated 
 *  free all memory requested in history module
*/
static env_map_t *env_vars;

static inline env_map_t *_init_env_vars() {
    env_map_t* _env_vars = (env_map_t*)malloc(sizeof(env_map_t));
    _env_vars->cap = CAP_BASE;
    _env_vars->keys = malloc(sizeof(const char *) * _env_vars->cap);
    _env_vars->vals = malloc(sizeof(const char *) * _env_vars->cap);
    _env_vars->cnt = 0;
    return _env_vars;
}

static inline int add_entry_to_env_vars
        (const char *__key, const char *__val) {
    int i;
    /* TODO: modify to strndup() */
    const char *key = strdup(__key);
    const char *val = strdup(__val);
    for (i = 0; i < env_vars->cnt; i++) {
        if (strncmp(key, env_vars->keys[i], 
            strlen(env_vars->keys[i])) == 0) {
            /* release the original var memmory */
            free(env_vars->vals[i]); 
            env_vars->vals[i] = val;
        }
    }
    if (env_vars->cnt == env_vars->cap) {
        /* expand the map */
        env_vars->cap *= 2;
        env_vars->keys = 
            realloc(env_vars->keys, sizeof(const char *) * env_vars->cap);
        if (!env_vars->keys) 
            return -1;
        env_vars->vals = 
            realloc(env_vars->vals, sizeof(const char *) * env_vars->cap);
        if (!env_vars->vals)   
            return -1;
    }
    env_vars->keys[env_vars->cnt] = key;
    env_vars->vals[env_vars->cnt] = val;
    ++(env_vars->cnt);

    return 0;
} 

static inline void print_entry(int index) {
    fprintf(stdout, "%s=%s\n", env_vars->keys[index],
                               env_vars->vals[index]);
    return ;
}

static inline void add_built_in_entries() {
    add_entry_to_env_vars("USER", "admin");
    return ;
}

extern int __printenv(const int *argc, const char *const argv[]);
extern int __setenv(const int *argc, const char *const argv[]);

extern void init_env_vars();
extern void _free_all_in_env();

#endif