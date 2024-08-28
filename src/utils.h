#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>

#define PATH_MAX 4096
#define SEQUENCE_CAP 8
#define STRING_CAP 1024
#define HISTORY_CAP 1024
#define HISTORY_FILE "shell_history"

#define RESET_ANSI "\033[0m"
#define ANSI_COLOR(color_code, s) "\033[" color_code "m" s RESET_ANSI

#define BLACK(s)   ANSI_COLOR("30", s)
#define RED(s)     ANSI_COLOR("31", s)
#define GREEN(s)   ANSI_COLOR("32", s)
#define YELLOW(s)  ANSI_COLOR("33", s)
#define BLUE(s)    ANSI_COLOR("34", s)
#define MAGENTA(s) ANSI_COLOR("35", s)
#define CYAN(s)    ANSI_COLOR("36", s)
#define WHITE(s)   ANSI_COLOR("37", s)


void    panic(const char *s);
pid_t   fork_panic(void);
void    pipe_panic(int pipefd[2]);
ssize_t read_panic(int fd, void* buf, size_t nbytes);
int     close_panic(int fd);
void    close_opened_fd(int fd);
pid_t   waitpid_panic(pid_t pid, int* stat_loc, int options);
char*   getcwd_panic(char* buf, size_t size);
void*   malloc_panic(size_t size);
void*   realloc_panic(void* ptr, size_t size);

bool    token_is_end_of_word(char c);
ssize_t editor_read_sequence(char* buffer);
ssize_t editor_read_line(char** lineptr, size_t* n, FILE* stream);
void    editor_clear_line(void);
void    editor_draw_cursor(int cursor, int offset);
void    editor_set_cursor_style(int style);
int     editor_print_prompt_line(void);
int     file_count_lines(const char* filename);
void    file_append_line(const char* filename, const char* str);

#endif /* UTILS_H */
