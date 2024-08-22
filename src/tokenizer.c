#include "tokenizer.h"


void TokenizerInit(struct Tokenizer* tokenizer, char* line) {
    tokenizer->token_count = 0;
    tokenizer->head = NULL;

    char* next = line;
    struct Token* last_token = NULL;

    for (;;) {
        while (*next == ' ' || *next == '\t' || *next == '\n') {
            ++next;
        }

        if (*next == '\0') {
            break;
        }

        struct Token* token = malloc_panic(sizeof(struct Token));
        token->start = next++;
        token->next = NULL;

        switch (*token->start) {
            case '<':
                token->type = TT_INFILE;
                break;
            case '>':
                token->type = TT_OUTFILE;
                break;
            case '|':
                token->type = TT_PIPE;
                break;
            default:
                token->type = TT_WORD;
                while (*next != '\0' && *next != ' ' && *next != '\t' && *next != '\n') {
                    ++next;
                }
        }

        token->len = next - token->start;

        if (last_token) {
            last_token->next = token;
        } else {
            tokenizer->head = token;
        }

        last_token = token;
        ++tokenizer->token_count;
    }
}

void TokenizerFree(struct Tokenizer* tokenizer) {
    struct Token* next = tokenizer->head;
    while (next) {
        struct Token* prev = next;
        next = next->next;
        free(prev);
    }
}
