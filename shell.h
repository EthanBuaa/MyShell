#ifndef SHELL_H_
#define SHELL_H_

/* Constants */
#define MAX_LINE (256)
#define MAX_ARGS (1024)

/* type declearation */
struct command {
    int     argc;
    int     fds[2];
    char    *argv[MAX_ARGS];
};

static inline struct command *init_cmd() {
    return (struct command *) 
        calloc(sizeof(struct command) + MAX_ARGS * sizeof(int), 1) ;
}

struct command_piped {
    struct command  **cmds;
    int             cmd_count;
};

/* alloctae memory for one input command*/
static inline struct command_piped *init_cmd_piped(int cmd_cnt) {
    return (struct command_piped *) 
        calloc(sizeof(struct command_piped) + cmd_cnt * sizeof(struct command *), 1);
}

/* Functions declaration */
extern const char *current_directory();

static struct command *parse_cmd(char *);
extern struct command_piped *parse_cmd_piped(char *);

static int exec_cmd(const struct command *, int (*pipes)[2]);
static inline close_pipes(int (*pipes)[2], int pipe_cnt) {
    int i;
    for (i = 0; i < pipe_cnt; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}

extern int exec_cmd_piped(struct command_piped *);

extern void flush_cmd_piped(struct command  *);

/** function called when shell is terminated 
 *  free all memory requested in shell module
*/
extern void _free_all_in_shell();        

#endif
