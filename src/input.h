#ifndef INPUT_H
#define INPUT_H

#include "utils.h"

#include <string.h>
#include <stdio.h>

#define STR_CAP 1024

struct Input {
    char str[STR_CAP];
    int cursor;
    int str_len;
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

#endif /* INPUT_H */
