#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

#include "built_in.h"

extern int handle_built_in(int* argc, char *argv[]) {
    int idx; 
    if ((idx = get_built_in_index(argv[0])) < 0) 
        return 0;
    
    int exec_ret = built_in_handler_map[idx](argc, argv);
    return exec_ret;
}

