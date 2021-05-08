#ifndef HISTORY_H_
#define HISTORY_H_

#define MAX_HISTORY_ENTRIES (512)

struct history {
    int cur;
    int entry_count;
    const char *entries[MAX_HISTORY_ENTRIES];
};

static struct history *historys;
static inline struct history *init_historys() {
    return calloc(sizeof(struct history), 1);
}

static inline void destroy_historys() {
    free(historys);
}

static inline void clear_entries_in_history() {
    int i;
    for (i = 0; i < historys->entry_count; i++) {
        free(historys->entries[i]);
    }
    historys->cur = historys->entry_count = 0;
}


extern int add_entry_to_history(const char * ) __THROW;
extern int __history(int * , char *[]);

/** function called when shell is terminated 
 *  free all memory requested in history module
*/
extern void _free_all_in_history();   

#endif