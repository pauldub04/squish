#include "utils.h"

void panic(const char *s) {
    fprintf(stderr, "[ERROR]: %s\n", s);
    exit(1);
}

pid_t fork_panic(void) {
    pid_t pid = fork();
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

ssize_t read_panic(int fd, void* buf, size_t nbytes) {
    ssize_t nread = read(fd, buf, nbytes);
    if (nread == -1) {
        panic("read");
    }
    return nread;
}

int close_panic(int fd) {
    int res = close(fd);
    if (res == -1) {
        panic("close");
    }
    return res;
}

void close_opened_fd(int fd) {
    if (fd == STDIN_FILENO || fd == STDOUT_FILENO || fd == STDERR_FILENO) {
        return;
    }
    close_panic(fd);
}

pid_t waitpid_panic(pid_t pid, int* stat_loc, int options) {
    pid_t res = waitpid(pid, stat_loc, options);
    if (res == -1) {
        panic("waitpid");
    }
    return res;
}

char* getcwd_panic(char* buf, size_t size) {
    char* res = getcwd(buf, size);
    if (res == NULL) {
        panic("getcwd");
    }
    return res;
}


void* malloc_panic(size_t size) {
    void* res = malloc(size);
    if (res == NULL) {
        panic("malloc");
    }
    return res;
}

void* realloc_panic(void* ptr, size_t size) {
    void* res = realloc(ptr, size);
    if (res == NULL) {
        panic("realloc");
    }
    return res;
}


ssize_t editor_read_sequence(char* buffer) {
    memset(buffer, 0, SEQUENCE_CAP);
    ssize_t nread = 0;
    while ((nread = read_panic(STDIN_FILENO, buffer, 1)) == 0);

    if (buffer[0] == '\033') {
        nread += read(STDIN_FILENO, buffer+1, SEQUENCE_CAP-1);
    }
    return nread;
}

ssize_t editor_read_line(char** lineptr, size_t* n, FILE* stream) {
    if (lineptr == NULL || n == NULL || stream == NULL) {
        return -1;
    }

    if (*lineptr == NULL) {
        *lineptr = malloc_panic(STRING_CAP);
        *n = STRING_CAP;
    }

    size_t pos = 0;
    int c;

    while ((c = fgetc(stream)) != EOF) {
        if (pos >= *n - 1) {
            size_t new_size = *n * 2;
            char *new_lineptr = realloc_panic(*lineptr, new_size);
            *lineptr = new_lineptr;
            *n = new_size;
        }

        (*lineptr)[pos++] = c;
        if (c == '\n') {
            break;
        }
    }

    if (pos == 0 && c == EOF) {
        return -1;
    }

    (*lineptr)[pos] = '\0';
    return pos;
}

void editor_clear_line(void) {
    printf("\033[2K\r");
}

void editor_draw_cursor(int cursor, int offset) {
    printf("\033[%dG", cursor+offset);
}

void editor_set_cursor_style(int style) {
    // Set the cursor style:
    // 0 = block, 4 = underline, 6 = beam, 1 = restore default
    printf("\033[%d q", style);
}

int editor_print_prompt_line(void) {
    char left_prompt[PATH_MAX];
    const char* right_prompt = " > ";

    getcwd_panic(left_prompt, sizeof(left_prompt));
    printf(BLUE("%s")GREEN("%s"), left_prompt, right_prompt);

    return strlen(left_prompt) + strlen(right_prompt);
}

int file_count_lines(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return 0;
    }

    int lines = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            ++lines;
        }
    }

    fclose(file);
    return lines;
}

void file_append_line(const char* filename, const char* str) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        return;
    }
    fprintf(file, "%s\n", str);
    fclose(file);
}

bool token_is_end_of_word(char c) {
    return
        c == '\0' || c == ' ' || c == '\t' || c == '\n' ||
        c == '|' || c == '&' || c == ';';
}
