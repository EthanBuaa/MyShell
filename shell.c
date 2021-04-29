#include "shell.h"

char *readLine(void) {
    char ch; 
    int i = 0;
    int buf_size = BUF_SIZE;
    char *line = (char *)malloc(buf_size * sizeof(char));

    /* throw error to main() */
    if (!line) { return NULL; }

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

        /** Error occurs in realloction 
         * handle error right away
        */
        if (!line) { 
            fprintf(stderr, 'error: realloc failed\n');
            exitWithCleanup(EXIT_FAILURE);
        } 

        line[i++] = ch;
    } 

    line[i] = '\0';
    return line;
}

void _clearHistory(History *htry) {
    int i;
    for (i = 0; i < htry->entry_count; i++) {
        free(htry->entries[i]);
    }
    free(htry);

    return ;
}

Command *_parseCommands(char *input) {
    int argv_count = 0;
    char *token;
    char **save_ptr;

    Command *cmd = calloc(1, sizeof(Command) + sizeof(char) * MAX_ARGS);

    if (!cmd) {
        fprintf(stderr, "error: memory alloc error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok_r(input, " ",save_ptr);
    while (token != NULL && argv_count < MAX_ARGS) {
        cmd->argv[argv_count++] = token;
        token = strtok_r(NULL, " ", save_ptr);
    }
    cmd->name = cmd->argv[0];
    cmd->argc = argv_count;

    return cmd;
}

CommandPiped *parseCommandsPiped(char *line) {
    int cmd_count = 0;
    char *ptr = line;
    char *token = NULL;
    char *save_ptr = NULL;

    while (*ptr) {
        if (*ptr == '|') { cmd_count++; }
        ptr++;
    }

    CommandPiped *cmd_p = calloc(1, sizeof(CommandPiped) + cmd_count * sizeof(Command));

    if (!cmd_p) {
        fprintf(stderr, "error: memory alloc error\n");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    token = strtok_r(line, "|", save_ptr);
    while (token && i < cmd_count) {
        cmd_p->cmds[i++] = _parseCommand(token);
        token = strtok_r(NULL, "|", save_ptr);
    }

    cmd_p->cmd_count = cmd_count;

    return cmd_p;
}

int exec_command(CommandPiped *cmd_p, Command *cmd, int (*pipes)[2]) {
    if (checkBuiltIn()) {
        return handleBuiltIn(cmd_p, cmd);
    }

    int pid;
    if ((pid = fork()) == 0) {
        int input_fd = cmd->fds[0];
		int output_fd = cmd->fds[1];

		// change input/output file descriptors if they aren't standard
		if (input_fd != -1 && input_fd != STDIN_FILENO)
			dup2(input_fd, STDIN_FILENO);

		if (output_fd != -1 && output_fd != STDOUT_FILENO)
			dup2(output_fd, STDOUT_FILENO);

		if (pipes != NULL) {
			int pipe_count = cmd_p->cmd_count - 1;
			close_pipes(pipes, pipe_count);
		}

		/* execute the command */
		execv(cmd->name, cmd->argv);

		/* execv returns only if an error occurs */
		fprintf(stderr, "error: %s\n", strerror(errno));

		/* cleanup in the child to avoid memory leaks */
		clear_history();
		free(history);
		free(pipes);
		free(input);
		cleanup_commands(cmds);

		if (parent_cmd != NULL) {
			free(parent_cmd);
			free(temp_line);
			free(parent_cmds);
		}


		/* exit from child so that the parent can handle the scenario*/
		_exit(EXIT_FAILURE);
    } else if (pid == 1) {

    } else {
        fprintf(stderr, "error: fork error\n");
		return 0;
    }
}

int exec_commands(CommandPiped *cmd_p) {
    int exec_ret;

    if (cmd_p->cmd_count == 1) {
        cmd_p->cmds[0]->fds[STDIN_FILENO] = STDIN_FILENO;
        cmd_p->cmds[0]->fds[STDOUT_FILENO] = STDOUT_FILENO;
        exec_ret = exec_command(cmd_p, cmd_p->cmds[0], NULL);
        wait(NULL);
    } else {
        /* excute command with pipeline */
        int pipe_count = cmd_p->cmd_count - 1;

        int i;
        for (i = 0; i < cmd_p->cmd_count; i++) {
            if (checkBuiltIn(cmd_p->cmds[i])) {
                fprintf(stderr, "error: no builtins in pipe\n");
				return 0;
            }
        }

        int (*pipes)[2] = calloc(1, pipe_count * sizeof(int[2]));
        if (!pipes) {
            fprintf(stderr, "error: memory alloc error\n");
            return 0;
        }

        /* create pipes and set file descriptors on commands */
		cmd_p->cmds[0]->fds[STDIN_FILENO] = STDIN_FILENO;
		for (i = 1; i < cmd_p->cmd_count; i++) {
			pipe(pipes[i-1]);
			cmds->cmds[i-1]->fds[STDOUT_FILENO] = pipes[i-1][1];
			cmds->cmds[i]->fds[STDIN_FILENO] = pipes[i-1][0];
		}
		cmd_p->cmds[pipe_count]->fds[STDOUT_FILENO] = STDOUT_FILENO;

		/* execute the commands */
		for (i = 0; i < cmd_p->cmd_count; i++)
			exec_ret = exec_command(cmd_p, cmds->cmds[i], pipes);

		close_pipes(pipes, pipe_count);

		/* wait for children to finish */
		for (i = 0; i < cmd_p->cmd_count; i++) { wait(NULL); }

		free(pipes);
	}

	return exec_ret;
    }
}

void flushCommands(CommandPiped *cmd_p) {
    int i;
    for (i = 0; i < cmd_p->cmd_count; i++) {
        free(cmd_p->cmds[i]);
    }
    free(cmd_p);

    return ;
}
