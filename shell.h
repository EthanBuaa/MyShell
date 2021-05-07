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
    char    *argv[MAX_ARGS];
    bool    bg;
    int     fds[2];
    char    *ifile;
    char    *ofile;
};

static inline struct command *init_cmd() {
    return (struct command *) 
        calloc(sizeof(struct command), 1) ;
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
static inline const char *current_directory __THROW () {
    char *buf, *cwd;
    if (!(buf = malloc(BUFSIZ))) 
        return NULL;
    if (!(cwd = getcwd(buf, BUFSIZ))) 
        return NULL;
    return cwd;
}
extern void print_prompt();

static struct command *parse_cmd(char *);
extern struct command_piped *parse_cmd_piped(char *);

static int exec_cmd(const struct command *);
static inline void close_pipes(int (*pipes)[2], int pipe_cnt) {
    int i;
    for (i = 0; i < pipe_cnt; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}

static inline bool is_background(struct command *cmd) {
    return (bool) strncmp(cmd->argv[cmd->argc - 1], "&", 1) == 0;
}

extern int exec_cmd_piped(struct command_piped *);

extern void flush_cmd_piped(struct command  *);

/** function called when shell is terminated 
 *  free all memory requested in shell module
*/
extern void _free_all_in_shell();        

#endif
