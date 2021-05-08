#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

#include "built_in.h"

extern int get_built_in_index(char *str) {
    int cnt = sizeof(built_in_strs) / sizeof(built_in_strs[0]);
    
    int i;
    for (i = 0; i < cnt; i++) {
        if (strncmp(str, built_in_strs[i], strlen(built_in_strs[i])) == 0) 
            return i;
    }
    return -1;
}

extern int handle_built_in(int index, int* argc, char *argv[]) {
    return built_in_handler_map[index](argc, argv);
}

extern void free_all_in_built_in() {
    _free_all_in_history();
    return ;
}
