#include "utils.h"

#include <string.h>

void panic(const char *s) {
    fprintf(stderr, "[ERROR]: %s\n", s);
    exit(1);
}

__pid_t fork_panic() {
    __pid_t pid = fork();
    if (pid == -1) {
        panic("fork");
    }
    return pid;
}

void pipe_panic(int pipefd[2]) {
    if (pipe(pipefd) == -1) {
        panic("pipe");
    }
}

void close_opened_fd(int fd) {
    if (STDIN_FILENO <= fd && fd <= STDERR_FILENO) {
        return;
    }
    close(fd);
}

void strreplace(char *s, const char *s1, const char *s2) {
    char *p = strstr(s, s1);
    if (p == NULL) {
        return;
    }
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    if (len1 != len2) {
        memmove(p + len2, p + len1, strlen(p + len1) + 1);
    }
    memcpy(p, s2, len2);
}
