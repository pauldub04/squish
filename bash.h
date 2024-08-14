#include "tokenizer.h"

#include <unistd.h>
#include <sys/wait.h>


void panic(const char *s) {
    fprintf(stderr, "[ERROR]: %s\n", s);
    exit(1);
}

struct Cmd {
    char** args;
};

void CmdInit(struct Cmd* cmd, struct Tokenizer* tokenizer) {
    cmd->args = (char**) malloc((tokenizer->token_count+1) * sizeof(char*));
    if (cmd->args == NULL) {
        panic("can not allocate memory");
    }
    cmd->args[tokenizer->token_count] = NULL;

    struct Token* token = tokenizer->head;
    int i = 0;

    while (token != NULL) {
        token->start[token->len] = '\0';
        cmd->args[i] = token->start;

        token = token->next;
        ++i;
    }
}

void CmdFree(struct Cmd* cmd) {
    if (cmd->args == NULL) {
        return;
    }
    free(cmd->args);
}

__pid_t fork_panic(void) {
    __pid_t pid = fork();
    if (pid == -1) {
        panic("fork");
    }
    return pid;
}

void Exec(struct Tokenizer* tokenizer) {
    if (tokenizer->head == NULL) {
        return;
    }

    struct Cmd cmd;
    CmdInit(&cmd, tokenizer);

    __pid_t pid = fork_panic();
    if (pid == 0) {
        int result = execvp(cmd.args[0], cmd.args);
        if (result < 0) {
            printf("Command not found\n");
        }
    } else {
        waitpid(pid, NULL, 0);
    }

    CmdFree(&cmd);
}
