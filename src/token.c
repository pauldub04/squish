#include "token.h"


struct Token* token_alloc(char* line) {
    struct Token* head = NULL;

    char* next = line;
    struct Token* last_token = NULL;

    while (1) {
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
                if (*next == '|') {
                    token->type = TT_OR;
                    ++next;
                } else {
                    token->type = TT_PIPE;
                }
                break;
            case ';':
                token->type = TT_SEMICOLON;
                break;
            case '&':
                if (*next == '&') {
                    token->type = TT_AND;
                    ++next;
                }
                break;
            default:
                token->type = TT_WORD;
                while (!token_is_end_of_word(*next)) {
                    ++next;
                }
        }

        token->len = next - token->start;

        if (last_token) {
            last_token->next = token;
        } else {
            head = token;
        }

        last_token = token;
    }
    return head;
}

void token_free(struct Token* head) {
    struct Token* next = head;
    while (next) {
        struct Token* prev = next;
        next = next->next;
        free(prev);
    }
}

bool token_is_separator(struct Token* token) {
    return
        token == NULL ||
        token->type == TT_PIPE || token->type == TT_SEMICOLON ||
        token->type == TT_AND || token->type == TT_OR;
}
