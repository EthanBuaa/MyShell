#ifndef SHELL_H_
#define SHELL_H_

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

/* Constants */
#define MAX_LINE (256)
#define MAX_ARGS (1024)
#define MAX_FILE_SIZE (64)

/* type declearation */
struct command {
    int     argc;
    char    *argv[MAX_ARGS + 1];    /* make sure argv[] ends with NULL pointer */
    bool    bg;
    char    *ifile;
    char    *ofile;
};

static inline struct command *init_cmd() {
    return (struct command *) 
        calloc(sizeof(struct command), 1);
}

struct command_piped {
    struct command  **cmds;
    int             cmd_count;
};

/* alloctae memory for one input command*/
static inline struct command_piped *init_cmd_piped(int cmd_cnt) {
    struct command_piped * _cmd_piped = 
        calloc(sizeof(struct command_piped), 1);
    _cmd_piped->cmds = malloc(sizeof(struct command*) * cmd_cnt);
    return _cmd_piped;
}

/* Functions declaration */
static inline const char *current_directory() {
    char *buf, *cwd;
    if ((buf = malloc(BUFSIZ)) == 0) 
        return NULL;
    if ((cwd = getcwd(buf, BUFSIZ)) == 0) 
        return NULL;
    return cwd;
}
extern void print_prompt();

extern struct command_piped *parse_cmd_piped(char *);

static inline bool is_background(struct command *cmd) {
    return (cmd->argc > 0)? 
        (bool) strncmp(cmd->argv[cmd->argc - 1], "&", 1) == 0 : false;
}

extern int exec_cmd_piped(struct command_piped *);

extern void flush_cmd_piped(struct command_piped  *);

/** function called when shell is terminated 
 *  free all memory requested in shell module
*/
extern void _free_all_in_shell();        

#endif
