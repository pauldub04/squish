#include "bash.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int GetCmd(char** line, size_t* maxlen) {
    printf("$ ");
    return getline(line, maxlen, stdin);
}

int main() {
    char* line = NULL;
    size_t maxlen = 0;
    setbuf(stdin, NULL);

    ssize_t len = 0;
    struct Tokenizer tokenizer;

    while ((len = GetCmd(&line, &maxlen)) > 0) {
        TokenizerInit(&tokenizer, line);

        Exec(&tokenizer);

        TokenizerFree(&tokenizer);
    }

    if (line) {
        free(line);
    }

    printf("\n");

    return 0;
}
