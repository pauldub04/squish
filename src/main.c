#define  _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>

#include "bash.h"

#define RESET "\033[0m"
#define RED   "\033[31m"
#define GREEN "\033[32m"
#define BLUE  "\033[34m"

char *strreplace(char *s, const char *s1, const char *s2) {
    char *p = strstr(s, s1);
    if (p != NULL) {
        size_t len1 = strlen(s1);
        size_t len2 = strlen(s2);
        if (len1 != len2)
            memmove(p + len2, p + len1, strlen(p + len1) + 1);
        memcpy(p, s2, len2);
    }
    return s;
}

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
