#ifndef BASH_H
#define BASH_H

#include "token.h"

#include <stdbool.h>

enum ErrorCode {
    EC_NONE = 0,
    EC_IO,
    EC_SYNTAX,
};

struct Cmd {
    char** args;
    char* in_file;
    char* out_file;
    struct Cmd* next;
    enum TokenType separator;
    enum ErrorCode error_code;
};

struct Cmd* cmd_alloc(struct Token* cmd_head, int pipeline_cnt);
void cmd_free(struct Cmd* cmd);

void print_error(enum ErrorCode error_code);
void print_error_list(struct Cmd* cmd_head); 
bool check_error_list(struct Cmd* cmd_head);

void exec_cmd(struct Cmd* cmd, int in_fd, int out_fd);
void run_cmd(struct Token* token_head);

#endif /* BASH_H */
