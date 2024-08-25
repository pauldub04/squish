#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdbool.h>


void panic(const char *s);

pid_t fork_panic(void);
void pipe_panic(int pipefd[2]);
ssize_t read_panic(int fd, void* buf, size_t nbytes);
int close_panic(int fd);
void close_opened_fd(int fd);
pid_t waitpid_panic(pid_t pid, int* stat_loc, int options);

void* malloc_panic(size_t size);
void* realloc_panic(void* ptr, size_t size);

void clear_line(void);
void draw_cursor(int cursor);
void set_cursor_style(int style);

bool is_end_of_word(char c);

#endif /* UTILS_H */
