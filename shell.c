#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>

#include "built_in.h"
#include "shell.h"

const char *current_directory() {
    /* throw error to the caller */
    char *buf = (char *) malloc(BUFSIZ);
    if (!buf) { return NULL; }
    const char *cwd = getcwd(buf, BUFSIZ);
    if (!cwd) { return NULL; }

    return cwd;
}
struct command *parse_cmd(char *input) {
    int arg_count = 0;
    char *token = NULL;
    char **save_ptr = NULL;

    struct command *cmd = init_cmd();

    if (!cmd) {
        fprintf(stderr, "error: memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok_r(input, " ", save_ptr);
    while (token != NULL && arg_count < MAX_ARGS) {
        cmd->argv[arg_count++] = token;
        token = strtok_r(NULL, " ", save_ptr);
    }
    cmd->argc = arg_count;

    return cmd;
}

struct command_piped *parse_cmd_piped(char *line) {
    int cmd_cnt = 0;
    char *ptr = line;
    char *token = NULL;
    char *save_ptr = NULL;

    while (*ptr) {
        if (*ptr == '|') { cmd_cnt++; }
        ptr++;
    }

    struct command_piped *cmd_p = 
        init_cmd_piped(cmd_cnt);

    if (!cmd_p) {
        fprintf(stderr, "error: memory alloc error\n");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    token = strtok_r(line, "|", save_ptr);
    while (token && i < cmd_cnt) {
        cmd_p->cmds[i++] = parse_command(token);
        token = strtok_r(NULL, "|", save_ptr);
    }

    cmd_p->cmd_count = cmd_cnt;

    return cmd_p;
}

/**
 * FIXME: memory control over child process  
 */
int exec_cmd(const struct command *cmd, int (*pipes)[2]) {
    if (is_built_in(cmd->argv[0])) {
        return handle_built_in(&cmd->argc, cmd->argv);
    }

    pid_t pid;
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
		execv(cmd->argv[0], cmd->argv);

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
        /* in parent */
    } else {
        fprintf(stderr, "error: fork error\n");
		return 0;
    }
}

/**
 * TODO: I/O redirect 
*/
int exec_cmd_piped(struct command_piped *cmd_p) {
    int exec_ret;

    cmd_p->cmds[0]->fds[STDIN_FILENO] = STDIN_FILENO;
    cmd_p->cmds[cmd_p->cmd_count - 1]->fds[STDOUT_FILENO] = STDOUT_FILENO;    

    if (cmd_p->cmd_count == 1) {
        exec_ret = exec_cmd(cmd_p->cmds[0], NULL);
        wait(NULL);
    } else {
        /* excute command with pipeline */
        int pipe_count = cmd_p->cmd_count - 1;

        int (*pipes)[2] = calloc(1, pipe_count * sizeof(int[2]));
        if (!pipes) {
            fprintf(stderr, "error: memory allocation error\n");
            return 0;
        }

        int i;
        /* create pipes and set file descriptors on commands */
		for (i = 1; i < cmd_p->cmd_count; i++) {
			pipe(pipes[i - 1]);
            if (pipe < 0) {
                fprintf(stderr, "error: pipe build error between cmd %d and cmd %d\n",
                        i - 1, i);
                return 0;
            }

			cmd_p->cmds[i - 1]->fds[STDOUT_FILENO] = pipes[i - 1][1];
			cmd_p->cmds[i]->fds[STDIN_FILENO] = pipes[i - 1][0];
		}
		
		/* execute the commands */
		for (i = 0; i < cmd_p->cmd_count; i++)
			exec_ret = exec_command(cmd_p, cmd_p->cmds[i], pipes);

		close_pipes(pipes, pipe_count);

		/* wait for children to finish */
		for (i = 0; i < cmd_p->cmd_count; i++) 
            wait(NULL);

		free(pipes);
	}

	return exec_ret;
}

void flush_cmd(struct command_piped *cmd_p) {
    int i;
    for (i = 0; i < cmd_p->cmd_count; i++) {
        free(cmd_p->cmds[i]);
    }
    free(cmd_p);

    return ;
}
