#include "main.h"

__attribute((constructor)) void __init__() {
    History *history = (History*) malloc(sizeof(History));
}

int 
main(int argc, char **argv) {
    int exec_ret;

    for ( ; ; ) {
        fputs("root:", stdout);
        printDir();
        fputs("# ", stdout);

        line = readLine();

        if (!line) { 
            /* user entered ctrl+D, exit gracefully */
			CLEAR_N_EXIT(EXIT_FAILURE);
        }

        if (strlen(line) > 0 && !isBlank(line)) {
            char *line_copy = strndup(line, MAX_LINE);

            CommandPiped *commands = 
                parseCommandsPiped(line);

            /* add pipeline cmds & other commands to history */
			
            free(line_copy);
            line_copy = NULL;

            exec_ret = execCommands(commands);
            flushCommands(commands);
        }

        free(line);

        /* handle exit singal */
        if (exec_ret == -1) { break; }
    }
    
    /* exit with cleanup */
    CLEAR_N_EXIT(EXIT_SUCCESS);
    return 0;
}