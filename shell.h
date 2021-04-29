#ifndef SHELL_H_
#define SHELL_H_

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <errno.h>
#include <string.h>

/* Constants */
#define MAX_LINE (256)
#define BUF_SIZE (4096)
#define MAX_ARGS (1024)
#define MAX_HISTORY_ENTRIES (1000)

/* type declearation */
struct command {
    int argc;
    char *name;
    char *argv[MAX_ARGS];
    int fds[2];
};

struct commandPiped {
    int cmd_count;
    struct comand *cmds[];
};

struct history {
    int cur;
    int entry_count;
    char **entries;
};

typedef struct command Command;
typedef struct commandPiped CommandPiped;
typedef struct history History;

/* Functions declaration */
char *readLine(void );

void _clearHistory(History * );

Commands *parseCommandsPiped(char *);
Commands *_parseCommand(char *);
void flushCommands(Commands *);

        

#endif
