#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>

#include <assert.h>

#include "built_in.h"
#include "shell.h"

/* declare static functions */
static struct command *parse_cmd(char *);
static int exec_cmd(const struct command *);


void print_prompt() {
    fputs("admin:", stdout);
    const char *cwd = current_directory();
    if (!cwd) {
        fprintf(stderr, "error: failed to get current working path.\n");
        exit(EXIT_FAILURE);
    }
    fputs(cwd, stdout);
    free((void *)cwd);
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
        
        /**
         * NOTE: pattern recognization Algorithm here will choose
         * the last IO redirection as the final option
        */
        switch (*token) {
        case '<':
            if (token[1] == '\0')
                token = cmd->argv[++i];
            else 
                ++token; 
            cmd->ifile = token;

            if (!cmd->ifile || cmd->ifile[0] == '\0') 
                err = 1;
            break;
        case '>':
            if (token[1] == '\0') 
                token = cmd->argv[++i];
            else 
                ++token; 
            cmd->ofile = token;

            if (!cmd->ofile || cmd->ofile[0] == '\0') 
                err = 1;
            break;
        default:
            cmd->argv[j++] = cmd->argv[i];
            break;
        }

        if (err) {
            /* throw to caller */
            return NULL;
        }
    }
    cmd->argc = j;
    cmd->argv[cmd->argc] = NULL;    /* terminate argv[] with NULL pointer */

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
    cmd_cnt++;

    struct command_piped *cmd_p = 
        init_cmd_piped(cmd_cnt);

    if (!cmd_p) {
        fprintf(stderr, "error: memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    token = strtok_r(line, "|", &save_ptr);
    while (token && i < cmd_cnt) {
        if ((cmd_p->cmds[i++] = parse_cmd(token)) == NULL) {
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
    // int idx;
    // if ((idx = get_built_in_index(cmd->argv[0])) >= 0 &&
    //      parent_process_relied(idx)) {
    //     /** 
    //      * built-in cmds excuted here can't support redirection
    //      * include: history, cd, exit
    //      * FIXME: make history support IO redirection
    //      */
    //     return handle_built_in(idx, &cmd->argc, cmd->argv);
    // }

    // int ifd, ofd;
    // pid_t pid = fork();
    // if (pid == 0) {
	// 	ifd = (cmd->ifile)? 
    //             open(cmd->ifile, O_RDONLY) : cmd->fds[0];
    //     if (ifd < 0) {
    //         fprintf(stderr, "error: failed in open %s\n.", cmd->ifile);
    //         _exit(EXIT_FAILURE);
    //     }
    //     if (ifd != STDIN_FILENO) {
    //         /* change input file descriptors if they aren't standard */
	// 		if (dup2(ifd, STDIN_FILENO) < 0) {
    //             fprintf(stderr, "error: dup2 failed.\n");
    //             _exit(EXIT_FAILURE);
    //         }
    //         close(ifd);
    //     }

    //     /**
    //      * mode = 0644 represents the access:
    //      * read/write for owner
    //      * read-only for group and others
    //     */
    //     ofd = (cmd->ofile)?
    //             open(cmd->ofile, O_WRONLY | O_CREAT | O_TRUNC, 0644) : cmd->fds[1];
    //     if (ofd < 0) {
    //         fprintf(stderr, "error: failed in open %s\n.", cmd->ofile);
    //         _exit(EXIT_FAILURE);    
    //     }
    //     if (ofd != STDOUT_FILENO) {
	// 		/* change input/output file descriptors if they aren't standard */
    //         if (dup2(ofd, STDOUT_FILENO) < 0) {
    //             fprintf(stderr, "error: dup2 failed.\n");
    //         }
    //         close(ofd);
    //     }
		
	// 	/* execute the command */
    //     if (idx >= 0) {
    //         /**
    //          * handle built-in commands in child process
    //          * such as: 
    //          * cmds with IO (help),
    //          * or cmds not manipulate ancestor process(process name: mysh)        
    //         */
    //         int status = handle_built_in(idx, &cmd->argc, cmd->argv) < 0;
    //         _exit(status);
    //     }
	// 	execvp(cmd->argv[0], cmd->argv);
    //     /** 
    //      * TODO: handle errors here 
    //      * execv returns only if an error occurs 
	// 	 * exit from child so that the parent can handle the scenario 
    //      */
    //     fprintf(stderr, "error: failed in execvp().\n");
    //     _exit(EXIT_FAILURE);
    // } 
    // /* parent process continue here */
    // int status;
    // if (cmd->bg) {
    //     /* TODO: record in list of background jobs */
    //     /* background process, dont't wait for child to finish */
    //     assert(0);
    // } else {
    //     /** otherwise block until child process is finished 
    //      * catch error in child process 
    //      */
    //     waitpid(pid, &status, 0);
    //     /* throw errors to caller */
    //     return WIFEXITED(status)? 
    //         WEXITSTATUS(status) : -WTERMSIG(status);    
    // }

    return 0;
}

int exec_cmd_piped(struct command_piped *cmd_p) {
    if (cmd_p->cmd_count < 1) 
        return 1;

    int exec_ret = 0;

    int itemp = dup(STDIN_FILENO);
    int otemp = dup(STDOUT_FILENO);

    int i;
    int idx;
    int ofd;
    int ifd = dup(itemp);
    
    int fdpipe[2];
    pid_t pid;
    
    for (i = 0; i < cmd_p->cmd_count; i++) {
        ifd = (cmd_p->cmds[i]->ifile)?
            open(cmd_p->cmds[i]->ifile, O_RDONLY) : ifd;  
        if (ifd < 0) {
            fprintf(stderr, "failed to open %s.\n", 
                    cmd_p->cmds[i]->ifile);
            exec_ret = 1;
            break;        
        }  
        dup2(ifd, STDIN_FILENO);
        close(ifd);

        if (i < cmd_p->cmd_count - 1) {
            if (pipe(fdpipe) < 0) {
                fprintf(stderr, "error: pipe build error between %s and cmd %s\n",
                        cmd_p->cmds[i - 1]->argv[0], cmd_p->cmds[i]->argv[1]);
                exec_ret = -1; 
                break;
            }
            ofd = fdpipe[1];
            ifd = fdpipe[0];
            if (cmd_p->cmds[i]->ofile) {
                /* write dummy msg to pipe to avoid block of child process*/
                write(ofd, NULL, 0);    
                close(ofd);
            }
        } else {
            ofd = dup(otemp);
        }
        ofd = (cmd_p->cmds[i]->ofile)?
                open(cmd_p->cmds[i]->ofile, O_WRONLY | O_CREAT | O_TRUNC, 0644) : 
                ofd;
        if (ofd < 0) {
            fprintf(stderr, "failed to open %s.\n", 
                    cmd_p->cmds[i]->ofile);
            exec_ret = 1;
            break;        
        }

        dup2(ofd, STDOUT_FILENO);
        close(ofd);
        
        if ((idx = get_built_in_index(cmd_p->cmds[i]->argv[0])) >= 0 && 
             handle_built_in(idx, &cmd_p->cmds[i]->argc, 
                            (const char* const*) cmd_p->cmds[i]->argv) < 0) {
            return -1;
        }

        pid = fork();
        if (pid == 0) {
            execvp(cmd_p->cmds[i]->argv[0], cmd_p->cmds[i]->argv);
            /** 
             * TODO: handle errors here 
             * execv returns only if an error occurs 
             * exit from child so that the parent can handle the scenario 
             */
            _exit(EXIT_FAILURE);
        }
    }
    
    dup2(itemp, STDIN_FILENO);
    dup2(otemp, STDOUT_FILENO);
    close(itemp);
    close(otemp);
	
    if (!cmd_p->cmds[cmd_p->cmd_count - 1]->bg) {
        waitpid(pid, NULL, 0);
    }

	return exec_ret;
}

/**
 * This function free the memory of struct command_piped recursivly
 * Note: you might take it wrong for there is no free() 
 * for the tokenized line. 
 * In fact, free(line) will be called right 
 * after flush_cmd_piped in main.c 
 */
void flush_cmd_piped(struct command_piped *cmd_p) {
    int i;
    for (i = 0; i < cmd_p->cmd_count; i++) {
        free(cmd_p->cmds[i]);
    }
    free(cmd_p->cmds);
    free(cmd_p); 

    return ;
}

void _free_all_in_shell() {
    _free_all_in_built_in();
    return ;
}
