#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>

#include "built_in.h"
#include "shell.h"

void print_prompt() {
    fputs("user:", stdout);
    char *cwd = current_directory();
    if (!cwd) {
        fprintf(stderr, "error: failed to get current working path.\n");
        exit(EXIT_FAILURE);
    }
    fputs(cwd, stdout);
    free(cwd);
    fputs("$ ", stdout);

    return ;
}

struct command *parse_cmd(char *input) {
    int arg_count = 0;
    char *token = NULL;
    char *separators = " \t\n";
    char *save_ptr = NULL;

    struct command *cmd = init_cmd();

    if (!cmd) {
        fprintf(stderr, "error: memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok_r(input, separators, &save_ptr);
    while (token != NULL && arg_count < MAX_ARGS) {
        cmd->argv[arg_count++] = token;
        token = strtok_r(NULL, separators, &save_ptr);
    }
    cmd->argc = arg_count;

    int i, j;
    bool err;
    for (i = 0, j = 0; i < cmd->argc; i++) {
        token = cmd->argv[i];
        err = false;
        if (!token)
            break;
        
        switch (*token) {
        case '<':
            if (token[1] == '\0')
                token = cmd->argv[i++];
            else 
                ++token;
            cmd->ifile = token;

            if (!cmd->ifile || cmd->ifile[0] == '\0') 
                err = 1;
            break;
        case '>':
            if (token[1] == '\0') 
                token = cmd->argv[i++];
            else 
                ++token;
            cmd->ofile = token;

            if (!cmd->ofile || cmd->ofile[0] == '\0') 
                err = 1;
            break;
        default:
            cmd->argv[j++] = cmd->argv[i++];
            break;
        }

        if (err) {
            /* throw to caller */
            return NULL;
        }
    }
    cmd->argc = j;

    cmd->bg = is_background(cmd);
    if (cmd->bg) 
        --(cmd->argc);

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
        fprintf(stderr, "error: memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    token = strtok_r(line, "|", &save_ptr);
    while (token && i < cmd_cnt) {
        if (!(cmd_p->cmds[i++] = parse_cmd(token))) {
            fprintf(stderr, "failed in parsing %s.\n", token);
            return NULL;
        }
        token = strtok_r(NULL, "|", &save_ptr);
    }
    cmd_p->cmd_count = cmd_cnt;

    return cmd_p;
}

/**
 * this function handle IO exception and dup2 error locally
 * __throw__ return value of child process to caller 
 */
int exec_cmd(const struct command *cmd) {
    if (is_built_in(cmd->argv[0])) {
        return handle_built_in(&cmd->argc, cmd->argv);
    }

    int ifd, ofd;
    pid_t ch_pid = fork();
    if (ch_pid == 0) {
		ifd = (cmd->ifile)? 
                open(cmd->ifile, O_RDONLY) : cmd->fds[0];
        if (ifd < 0) {
            fprintf(stderr, "error: failed in open %s\n.", cmd->ifile);
            _exit(EXIT_FAILURE);
        }
        if (ifd != STDIN_FILENO) {
            /* change input file descriptors if they aren't standard */
			if (dup2(ifd, STDIN_FILENO) < 0) {
                fprintf(stderr, "error: dup2 failed.\n");
                _exit(EXIT_FAILURE);
            }
            close(ifd);
        }

        ofd = (cmd->ofile)?
                open(cmd->ofile, O_WRONLY | O_CREAT | O_TRUNC) : cmd->fds[1];
        if (ofd < 0) {
            fprintf(stderr, "error: failed in open %s\n.", cmd->ofile);
            _exit(EXIT_FAILURE);    
        }
        if (ofd != STDOUT_FILENO) {
			/* change input/output file descriptors if they aren't standard */
            if (dup2(cmd->fds[1], STDOUT_FILENO) < 0) {
                fprintf(stderr, "error: dup2 failed.\n");
            }
            close(ofd);
        }
		
		/* execute the command */
		execv(cmd->argv[0], cmd->argv);
        /* TODO: handle errors here */

        /** execv returns only if an error occurs 
		 * exit from child so that the parent can handle the scenario 
         */
        _exit(EXIT_FAILURE);
    } 
    /* parent process continue here */
    int status;
    close(cmd->fds[0]);
    if (cmd->bg) {
        /* TODO: record in list of background jobs */
        /* background process, dont't wait for child to finish*/
    } else {
        /** otherwise block until child process is finished 
         * catch error in child process 
         */
        waitpid(ch_pid, &status, 0);
        /* throw errors to caller */
        return WIFEXITED(status)? 
            WEXITSTATUS(status) : -WTERMSIG(status);
    }

    return 0;
}

int exec_cmd_piped(struct command_piped *cmd_p) {
    int exec_ret;

    cmd_p->cmds[0]->fds[STDIN_FILENO] = STDIN_FILENO;
    cmd_p->cmds[cmd_p->cmd_count - 1]->fds[STDOUT_FILENO] = STDOUT_FILENO;    

    if (cmd_p->cmd_count == 1) {
        exec_ret = exec_cmd(cmd_p->cmds[0]);
        // wait(NULL);
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
			if (pipe(pipes[i - 1]) < 0) {
                fprintf(stderr, "error: pipe build error between cmd %d and cmd %d\n",
                        i - 1, i);
                return 0;
            }

			cmd_p->cmds[i - 1]->fds[STDOUT_FILENO] = pipes[i - 1][1];
			cmd_p->cmds[i]->fds[STDIN_FILENO] = pipes[i - 1][0];
		}
		
		/* execute the commands */
		for (i = 0; i < cmd_p->cmd_count; i++) {
			exec_ret = exec_cmd(cmd_p->cmds[i]);
            /* TODO: handle exec_ret here */
        }
		close_pipes(pipes, pipe_count);

		/* wait for children to finish */
		// for (i = 0; i < cmd_p->cmd_count; i++) 
        //     wait(NULL);

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
