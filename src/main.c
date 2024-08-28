#include "token.h"
#include "editor.h"
#include "exec.h"

#include <stdio.h>


int main(void) {
    char* line = NULL;
    size_t maxlen = 0;

    ssize_t len = 0;

    while ((len = get_cmd(&line, &maxlen)) >= 0) {
        struct Token* token_head = token_alloc(line);
        run_cmd(token_head);
        token_free(token_head);
    }

    if (line) {
        free(line);
    }

    printf("\n");

    return 0;
}
