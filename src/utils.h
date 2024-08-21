#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>


void panic(const char *s);

__pid_t fork_panic();
void pipe_panic(int pipefd[2]);
void close_opened_fd(int fd);
ssize_t read_panic(int fd, void* buf, size_t nbytes);

void strreplace(char *s, const char *s1, const char *s2);

void clear_line();
void draw_cursor(int cursor);
void set_cursor_style(int style);

#endif /* UTILS_H */
