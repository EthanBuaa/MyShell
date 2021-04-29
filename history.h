#ifndef HISTORY_H_
#define HISTORY_H_

#define MAX_HISTORY_ENTRIES (512)

/**
 * TODO: add history commands
*/
struct history {
    int cur;
    int entry_count;
    char **entries;
};

/** function called when shell is terminated 
 *  free all memory requested in history module
*/
extern void free_all_in_history();   

#endif