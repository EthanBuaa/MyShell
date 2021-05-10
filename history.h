#ifndef HISTORY_H_
#define HISTORY_H_

#include <stdlib.h>

#define MAX_HISTORY_ENTRIES (512)

struct history {
    int cur;
    int entry_count;
    const char *entries[MAX_HISTORY_ENTRIES];
};

static struct history *historys;
static inline struct history *_init_historys() {
    struct history* _historys = malloc(sizeof(struct history));
    _historys->cur = -1;
    _historys->entry_count = 0;
    return _historys;
}

static inline void destroy_historys() {
    free((void *)historys);
}

static inline void clear_entries_in_history() {
    int i;
    for (i = 0; i < historys->entry_count; i++) {
        free((void *) historys->entries[i]);
    }
    historys->cur = -1;
    historys->entry_count = 0;
}

extern void init_historys();
extern int add_entry_to_history(const char * );
extern int __history(const int * , const char *const[] );

/** function called when shell is terminated 
 *  free all memory requested in history module
*/
extern void _free_all_in_history();   

#endif