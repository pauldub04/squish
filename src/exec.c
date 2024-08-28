#include "exec.h"

struct Cmd* cmd_alloc(struct Token* cmd_head, int pipeline_cnt) {
    struct Cmd* cmd = malloc_panic(sizeof(struct Cmd));

    cmd->args = NULL;
    cmd->in_file  = NULL;
    cmd->out_file = NULL;
    cmd->next = NULL;
    cmd->separator = TT_SEMICOLON;
    cmd->error_code = EC_NONE;

    for (struct Token* token = cmd_head; token != NULL; token = token->next) {
        // here is real divider, not NULL
        if (token_is_separator(token)) {
            cmd->next = cmd_alloc(token->next, pipeline_cnt+1);
            cmd->separator = token->type;
            break;
        }
    }

    // check correctness -----------------------------------------------------
    int infile_cnt = 0;
    int outfile_cnt = 0;
    int word_cnt = 0;
    for (struct Token* token = cmd_head; !token_is_separator(token); token = token->next) {
        if (token->type == TT_OUTFILE || token->type == TT_INFILE) {
            if (token->next == NULL || token->next->type != TT_WORD) {
                cmd->error_code = EC_SYNTAX;
                return cmd;
            }
            if (token->type == TT_INFILE) {
                ++infile_cnt;
            } else if (token->type == TT_OUTFILE) {
                ++outfile_cnt;
            }
        }
        if (token->type == TT_WORD) {
            ++word_cnt;
        }
    }

    if (word_cnt == 0 || infile_cnt > 1 || outfile_cnt > 1) {
        cmd->error_code = EC_SYNTAX;
        return cmd;
    }

    if (infile_cnt > 0 && pipeline_cnt != 0) {
        cmd->error_code = EC_SYNTAX;
        return cmd;
    }

    if (outfile_cnt > 0 && cmd->next != NULL) {
        cmd->error_code = EC_SYNTAX;
        return cmd;
    }
    //------------------------------------------------------------------------

    // split all word tokens
    for (struct Token* token = cmd_head; !token_is_separator(token); token = token->next) {
        if (token->type == TT_WORD) {
            token->start[token->len] = '\0';
        }
    }

    int args_cnt = 0;
    for (struct Token* token = cmd_head; !token_is_separator(token); token = token->next) {
        if (token->type == TT_OUTFILE || token->type == TT_INFILE) {
            if (token->type == TT_INFILE) {
                cmd->in_file = token->next->start;
            } else if (token->type == TT_OUTFILE) {
                cmd->out_file = token->next->start;
            }
            token = token->next;
            continue;
        }
        ++args_cnt;
    }

    // because of NULL in the end
    cmd->args = (char**) malloc_panic((args_cnt+1) * sizeof(char*));

    int i = 0;
    for (struct Token* token = cmd_head; !token_is_separator(token); token = token->next) {
        if (token->type == TT_OUTFILE || token->type == TT_INFILE) {
            token = token->next;
            continue;
        }

        cmd->args[i] = token->start;
        ++i;
    }
    cmd->args[args_cnt] = NULL;

    return cmd;
}

void cmd_free(struct Cmd* cmd) {
    if (cmd == NULL) {
        return;
    }
    cmd_free(cmd->next);

    if (cmd->args != NULL) {
        free(cmd->args);
    }
    free(cmd);
}


void print_error(enum ErrorCode error_code) {
    switch (error_code) {
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

void print_error_list(struct Cmd* cmd_head) {
    enum ErrorCode error_code = EC_NONE;
    for (struct Cmd* cmd = cmd_head; cmd != NULL; cmd = cmd->next) {
        if (cmd->error_code > error_code) {
            error_code = cmd->error_code;
        }
    }
    if (error_code != EC_NONE) {
        print_error(error_code);
    }
}

bool check_error_list(struct Cmd* cmd_head) {
    for (struct Cmd* cmd = cmd_head; cmd != NULL; cmd = cmd->next) {
        if (cmd->error_code != EC_NONE) {
            return true;
        }
    }
    return false;
}


void exec_cmd(struct Cmd* cmd, int in_fd, int out_fd) {
    if (cmd == NULL) {
        return;
    }

    int next_in_fd  = STDIN_FILENO;
    int next_out_fd = STDOUT_FILENO;
    if (strcmp(cmd->args[0], "cd") == 0) {
        if (cmd->args[1] == NULL || chdir(cmd->args[1]) < 0) {
            printf("Command not found\n");
            return;
        }
        close_opened_fd(in_fd);
        close_opened_fd(out_fd);
        exec_cmd(cmd->next, next_in_fd, next_out_fd);
        return;
    }

    if (cmd->in_file != NULL) {
        in_fd = open(cmd->in_file, O_RDONLY); 
        if (in_fd < 0) {
            cmd->error_code = EC_IO;
            return;
        }
    }
    if (cmd->out_file != NULL) {
        out_fd = open(cmd->out_file, O_WRONLY | O_CREAT | O_TRUNC, 0666); 
        if (out_fd < 0) {
            cmd->error_code = EC_IO;
            return;
        }
    }

    if (cmd->separator == TT_PIPE && cmd->next != NULL) {
        int pipefd[2];
        pipe_panic(pipefd);
        out_fd = pipefd[1];
        next_in_fd = pipefd[0];
    }

    pid_t pid = fork_panic();
    if (pid == 0) {
        if (in_fd != STDIN_FILENO) {
            dup2(in_fd, STDIN_FILENO);
        }
        int saved_stdout = -1;
        if (out_fd != STDOUT_FILENO) {
            saved_stdout = dup(STDOUT_FILENO);
            dup2(out_fd, STDOUT_FILENO);
        }

        int result = execvp(cmd->args[0], cmd->args);
        if (result < 0) {
            dup2(saved_stdout, STDOUT_FILENO);
            printf("Command not found\n");
            exit(1);
        }
    } else {
        int status;
        waitpid_panic(pid, &status, 0);

        close_opened_fd(in_fd);
        close_opened_fd(out_fd);

        if (cmd->separator == TT_OR || cmd->separator == TT_AND) {
            if (WIFEXITED(status)) {
                int exit_status = WEXITSTATUS(status);
                if (exit_status == 0) {
                    if (cmd->separator == TT_OR) {
                        return;
                    }
                } else {
                    if (cmd->separator == TT_AND) {
                        return;
                    }
                }
            } else {
                return;
            }
        }
        exec_cmd(cmd->next, next_in_fd, next_out_fd);
    }
}

void run_cmd(struct Token* token_head) {
    if (token_head == NULL) {
        return;
    }

    // getting list of commands
    struct Cmd* cmd_head = cmd_alloc(token_head, 0);

    if (!check_error_list(cmd_head)) {
        exec_cmd(cmd_head, STDIN_FILENO, STDOUT_FILENO);
    }

    if (check_error_list(cmd_head)) {
        print_error_list(cmd_head);
    }

    cmd_free(cmd_head);
}
