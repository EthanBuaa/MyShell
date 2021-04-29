#ifndef BUILT_IN_H_
#define BUILT_IN_H_

typedef int (*built_in_handler_t)(int *, char **);

struct built_in_handler_set {
    built_in_handler_t cd;
    built_in_handler_t exit;
};

static inline int __exit(int *argc, char **argv) {
    return -1;
}

static int __cd(int *argcm, char **argv);

static const 
    struct built_in_handler_set built_in_handler = {    
    .cd    = &__cd,                                      
    .exit  = &__exit,                                    
};

extern int is_built_in(char *argv0);
extern int handle_built_in(int* argc, char **argv);


#endif