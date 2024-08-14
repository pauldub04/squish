#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Token {
    char* start;
    size_t len;
    struct Token* next;
};

struct Tokenizer {
    size_t token_count;
    struct Token* head;
};

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

        char* start = next;
        while (*next != '\0' && *next != ' ' && *next != '\t' && *next != '\n') {
            ++next;
        }

        struct Token* token = (struct Token*)malloc(sizeof(struct Token));
        token->start = start;
        token->len = next - start;
        token->next = NULL;

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
