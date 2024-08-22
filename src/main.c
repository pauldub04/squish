#include "bash.h"
#include "editor.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(void) {
    char* line = NULL;
    size_t maxlen = 0;

    ssize_t len = 0;
    struct Tokenizer tokenizer;

    while ((len = get_cmd(&line, &maxlen)) > 0) {
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
