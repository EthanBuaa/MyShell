#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "shell.h"
#include "history.h"

#define clear_and_exit(status)      \
do {                                \
    _clear_all_history();           \
    _clear_all_shell();             \
    exit(status);                   \
} while (0)

static char *line;
static inline is_blank(char* str) {
    char *p = str;
    while (*p) {
        if (!isspace(*p)) {
            return 0;
        }
    }
    return 1;
}

static inline char *read_line(void) {
    char ch; 
    int i = 0;
    int buf_size = BUFSIZ;
    char *line = (char *)malloc(buf_size * sizeof(char));

    if (!line) 
        return NULL; 

    while ((ch = getchar()) != '\n') {
        if (ch == EOF) { 
            /* User enter ctrl + d */
            free(line);
            return NULL;
        }
        if (i >= buf_size) {
            buf_size = buf_size * 2;
            line = (char *)realloc(line, buf_size * sizeof(char));
        }

        if (!line) 
            return NULL;

        line[i++] = ch;
    } 

    line[i] = '\0';
    return line;
}

int main(int argc, char **argv) {
    int exec_ret;

    for (;;) {
        fputs("root:", stdout);
        printCurrentDirectory(stdout);
        fputs("# ", stdout);

        line = read_line();

        if (!line) { 
            /* user entered ctrl+D, exit gracefully */
			clear_and_exit(EXIT_FAILURE);
        }

        if (strlen(line) > 0 && !isBlank(line)) {
            char *line_copy = strndup(line, MAX_LINE);

            struct command_piped *cmd_p = 
                parse_cmd_piped(line);

            /* TODO: add pipeline cmds & other commands to history */
			
            free(line_copy); line_copy = NULL;

            exec_ret = exec_cmd_piped(cmd_p);
            flush_cmd_piped(cmd_p);
        }

        free(line);

        /* handle exit singal */
        if (exec_ret == -1) 
            break;
    }
    
    /* exit with cleanup */
    clear_and_exit(EXIT_SUCCESS);
    return 0;
}