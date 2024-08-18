#define  _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>

#include "bash.h"
#include "utils.h"

#define RESET "\033[0m"
#define RED   "\033[31m"
#define GREEN "\033[32m"
#define BLUE  "\033[34m"


int GetCmd(char** line, size_t* maxlen) {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        fprintf(stderr, BLUE"%s\n", cwd);
        fprintf(stderr, GREEN"> "RESET);
    } else {
        perror("getcwd");
        exit(1);
    }
    return getline(line, maxlen, stdin);
}

int main() {
    char* line = NULL;
    size_t maxlen = 0;
    setbuf(stdin, NULL);

    ssize_t len = 0;
    struct Tokenizer tokenizer;

    while ((len = GetCmd(&line, &maxlen)) > 0) {
        strreplace(line, "la", "ls -la");
        strreplace(line, "ls", "ls --color=auto");
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
