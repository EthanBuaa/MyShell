#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <signal.h>
#include <errno.h>

#include "history.h"

int add_entry_to_history(const char *__line) {
    const char *line = strdup(__line);
    if (!line) 
        return -1;
    
    historys->cur = (historys->cur + 1) % MAX_HISTORY_ENTRIES;
    historys->entries[historys->cur] = line;
    if (historys->entry_count < MAX_HISTORY_ENTRIES) 
        ++(historys->entry_count);

    return 0;
}

int __history(const int *argc, const char *const argv[]) {
    int offset = historys->entry_count;
    if (*argc > 1) {
        /* case: history -c */
        if (strncmp(argv[1], "-c", 2) == 0) {
            /* strlen("-c") = 2 */
            clear_entries_in_history();
            return 0;
        }   

        /* case: history [n] */
        char *end_ptr;
        long offset_l;

        /* 10 represents decimal (base = 10) */
        offset_l = strtol(argv[1], &end_ptr, 10);
        if (end_ptr == argv[1]) {
            fprintf(stderr, "error: failed in convert to number.\n");
            return -1;
        }
        offset = (offset_l > offset)? 
                    offset : (int) offset_l;
    }

    /* output the wanted history entries */
    int i;
    int idx_base = 
        (historys->cur - offset + 1 + MAX_HISTORY_ENTRIES) % MAX_HISTORY_ENTRIES;
    int no_base = historys->entry_count - offset;
    for (i = 0; i < offset; i++) {
        fprintf(stdout, "%4d  %s\n", 
            no_base + i, historys->entries[(idx_base + i) % MAX_HISTORY_ENTRIES]);        
    }


    return 0;
}

void init_historys() {
    /* into following branch at first call */
    if (!historys) {
        historys = _init_historys();
        if (!historys) {
            fprintf(stderr, "error: memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }
    }
}

void _free_all_in_history() {
    clear_entries_in_history();
    free(historys);
    return ;
}