#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

enum TokenType {
    TT_WORD,
    TT_INFILE,
    TT_OUTFILE,
    TT_PIPE,
    TT_SEMICOLON,
    TT_AND,
    TT_OR,
};

struct Token {
    char* start;
    size_t len;
    enum TokenType type;
    struct Token* next;
};

struct Token* token_alloc(char* line);
void token_free(struct Token* token);

bool token_is_separator(struct Token* token);

#endif /* TOKENIZER_H */
