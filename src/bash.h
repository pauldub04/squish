#ifndef BASH_H
#define BASH_H

#include "tokenizer.h"

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

struct Cmd* CmdAlloc(struct Token* start, int pipeline_cnt);
void CmdFree(struct Cmd* cmd);

void run_cmd(struct Cmd* cmd, int in_fd, int out_fd);
void print_error(enum ErrorCode error_code);
bool check_error_list(struct Cmd* start);
void print_error_list(struct Cmd* start); 
void Exec(struct Tokenizer* tokenizer);

#endif /* BASH_H */
