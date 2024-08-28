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


void panic(const char *s);

pid_t fork_panic(void);
void pipe_panic(int pipefd[2]);
ssize_t read_panic(int fd, void* buf, size_t nbytes);
int close_panic(int fd);
void close_opened_fd(int fd);
pid_t waitpid_panic(pid_t pid, int* stat_loc, int options);
pid_t waitpid_panic(pid_t pid, int* stat_loc, int options);
char* getcwd_panic(char* buf, size_t size);

void* malloc_panic(size_t size);
void* realloc_panic(void* ptr, size_t size);

ssize_t read_sequence(char* buffer);
ssize_t read_line(char** lineptr, size_t* n, FILE* stream);
void clear_line(void);
void draw_cursor(int cursor, int offset);
void set_cursor_style(int style);
int print_prompt_line(void);
int count_lines_file(const char* filename);
void append_to_file(const char* filename, const char* str);
bool is_end_of_word(char c);

#endif /* UTILS_H */
