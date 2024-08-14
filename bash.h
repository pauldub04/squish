#include "tokenizer.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>


void panic(const char *s) {
    fprintf(stderr, "[ERROR]: %s\n", s);
    exit(1);
}

__pid_t fork_panic(void) {
    __pid_t pid = fork();
    if (pid == -1) {
        panic("fork");
    }
    return pid;
}

enum ErrorCode {
    EC_NONE = 0,
    EC_COMMAND_NF,
    EC_SYNTAX,
    EC_IO,
};

struct Cmd {
    char** args;
    int in_fd;
    int out_fd;
    enum ErrorCode error_code;
};

void CmdInit(struct Cmd* cmd, const struct Tokenizer* tokenizer) {
    cmd->in_fd  = STDIN_FILENO;
    cmd->out_fd = STDOUT_FILENO;

    // split all tokens
    for (struct Token* token = tokenizer->head; token != NULL; token = token->next) {
        if (token->type == TT_WORD) {
            token->start[token->len] = '\0';
        }
    }

    // check io count
    int infile_cnt = 0;
    int outfile_cnt = 0;
    for (struct Token* token = tokenizer->head; token != NULL; token = token->next) {
        if (token->type == TT_OUTFILE || token->type == TT_INFILE) {
            if (token->next == NULL || token->next->type != TT_WORD) {
                cmd->error_code = EC_SYNTAX;
                return;
            }
            if (token->type == TT_INFILE) {
                ++infile_cnt;
            } else if (token->type == TT_OUTFILE) {
                ++outfile_cnt;
            }
        }
    }

    if (infile_cnt > 1 || outfile_cnt > 1) {
        cmd->error_code = EC_SYNTAX;
        return;
    }

    int args_cnt = tokenizer->token_count;
    for (struct Token* token = tokenizer->head; token != NULL; token = token->next) {
        if (token->type == TT_OUTFILE || token->type == TT_INFILE) {
            if (token->type == TT_INFILE) {
                cmd->in_fd = open(token->next->start, O_RDONLY); 
                if (cmd->in_fd < 0) {
                    cmd->error_code = EC_IO;
                    return;
                }
            } else if (token->type == TT_OUTFILE) {
                cmd->out_fd = open(token->next->start, O_WRONLY | O_CREAT | O_TRUNC, 0666); 
                if (cmd->out_fd < 0) {
                    cmd->error_code = EC_IO;
                    return;
                }
            }
            args_cnt -= 2;
        }
    }

    cmd->args = (char**) malloc((args_cnt+1) * sizeof(char*));
    if (cmd->args == NULL) {
        panic("can not allocate memory");
    }

    int i = 0;
    for (struct Token* token = tokenizer->head; token != NULL; token = token->next) {
        if (token->type == TT_OUTFILE || token->type == TT_INFILE) {
            token = token->next;
            continue;
        }

        cmd->args[i] = token->start;
        ++i;
    }
    cmd->args[args_cnt] = NULL;

}

void CmdFree(struct Cmd* cmd) {
    if (cmd->in_fd != STDIN_FILENO) {
        close(cmd->in_fd);
    }
    if (cmd->out_fd != STDOUT_FILENO) {
        close(cmd->out_fd);
    }
    if (cmd->args != NULL) {
        free(cmd->args);
    }
}


void print_error(enum ErrorCode error_code) {
    switch (error_code) {
    case EC_COMMAND_NF:
        fprintf(stdout, "Command not found\n");
        break;
    case EC_SYNTAX:
        fprintf(stdout, "Syntax error\n");
        break;
    case EC_IO:
        fprintf(stdout, "I/O error\n");
        break;
    default:
        break;
    }
}

void run_cmd(struct Cmd* cmd) {
    __pid_t pid = fork_panic();

    if (pid == 0) {
        if (cmd->in_fd != STDIN_FILENO) {
            dup2(cmd->in_fd, STDIN_FILENO);
        }
        if (cmd->out_fd != STDOUT_FILENO) {
            dup2(cmd->out_fd, STDOUT_FILENO);
        }

        int result = execvp(cmd->args[0], cmd->args);
        if (result < 0) {
            cmd->error_code = EC_COMMAND_NF;
        }
    } else {
        waitpid(pid, NULL, 0);
    }
}


void Exec(struct Tokenizer* tokenizer) {
    if (tokenizer->head == NULL) {
        return;
    }

    struct Cmd cmd;
    CmdInit(&cmd, tokenizer);

    if (cmd.error_code == EC_NONE) {
        run_cmd(&cmd);
    }

    if (cmd.error_code != EC_NONE) {
        print_error(cmd.error_code);
    }

    CmdFree(&cmd);
}
