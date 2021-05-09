#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>

#include "shell.h"
#include "history.h"

#define clear_and_exit(status)      \
do {                                \
    _free_all_in_shell();           \
    _free_all_in_history();         \
    exit(status);                   \
} while (0)

static char *line;
static inline bool is_blank(char* str) {
    char *p = str;
    while (*p) {
        if (!isspace(*p)) 
            return false;
    }
    return true;
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

int main(int argc, char *argv[]) {
    int exec_ret;

    for (;;) {
        print_prompt();
        line = read_line();

        if (!line) { 
            /* user entered ctrl+D, exit gracefully */
			clear_and_exit(EXIT_SUCCESS);
        }

        if (strlen(line) > 0 && !is_blank(line)) {
            /* TODO: missing error inspection here */
            add_entry_to_history(line);
            
            struct command_piped *cmd_p = 
                parse_cmd_piped(line);

            if (!cmd_p) {
                fprintf(stdout, "invalid command.\n");
                continue;
            }

            exec_ret = exec_cmd_piped(cmd_p);
            flush_cmd_piped(cmd_p);
        }

        free(line);

        /* handle exit singal */
        if (exec_ret < 0) 
            break;
    }
    
    /* exit with cleanup */
    clear_and_exit(EXIT_SUCCESS);
    return 0;
}