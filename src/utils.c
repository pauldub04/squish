#include "utils.h"

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

ssize_t read_panic(int fd, void* buf, size_t nbytes) {
    ssize_t nread = read(fd, buf, nbytes);
    if (nread == -1) {
        panic("read");
    }
    return nread;
}


void clear_line() {
    printf("\033[2K\r");
}

void draw_cursor(int cursor) {
    printf("\033[%dG", cursor+3);
}

void set_cursor_style(int style) {
    // Set the cursor style:
    // 0 = block, 4 = underline, 6 = beam, 1 = restore default
    printf("\033[%d q", style);
}
