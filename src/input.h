#ifndef INPUT_H
#define INPUT_H

#include "utils.h"

#include <string.h>
#include <stdio.h>


struct Input {
    char str[STRING_CAP];
    int len;
    int cursor;
};

void input_init(struct Input* input);

void input_move_cursor(struct Input *input, int delta);
void input_move_cursor_word_left(struct Input *input);
void input_move_cursor_word_right(struct Input *input);

void input_insert(struct Input *input, char c);
void input_delete(struct Input *input);
void input_delete_word(struct Input *input);
void input_backspace(struct Input *input);
void input_backspace_word(struct Input *input);
void input_print(struct Input *input);

struct History {
    struct Input inputs[HISTORY_CAP];
    int len;
    int current;
};

void history_append(struct History* history, struct Input* input);
void history_init(struct History* history);

#endif /* INPUT_H */
