#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum TokenType {
    TT_WORD,
    TT_INFILE,
    TT_OUTFILE,
    TT_PIPE,
};

struct Token {
    char* start;
    size_t len;
    enum TokenType type;
    struct Token* next;
};

struct Tokenizer {
    size_t token_count;
    struct Token* head;
};

void TokenizerInit(struct Tokenizer* tokenizer, char* line);
void TokenizerFree(struct Tokenizer* tokenizer);

#endif /* TOKENIZER_H */
