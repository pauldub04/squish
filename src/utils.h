#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>


void panic(const char *s);

__pid_t fork_panic();
void pipe_panic(int pipefd[2]);
void close_opened_fd(int fd);

void strreplace(char *s, const char *s1, const char *s2);
