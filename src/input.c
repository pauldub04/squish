#include "input.h"

void input_init(struct Input* input) {
    memset(input->str, 0, STR_CAP);
    input->cursor = 0;
    input->str_len = 0;
}

void input_move_cursor(struct Input *input, int delta) {
    input->cursor += delta;
    if (input->cursor < 0) {
        input->cursor = 0;
    }
    if (input->cursor > input->str_len) {
        input->cursor = input->str_len;
    }
}

void input_move_cursor_word_left(struct Input *input) {
    if (input->cursor <= 0) {
        return;
    }
    while (input->cursor > 0 && input->str[input->cursor-1] == ' ') {
        input_move_cursor(input, -1);
    }
    while (input->cursor > 0 && input->str[input->cursor-1] != ' ') {
        input_move_cursor(input, -1);
    }
}

void input_move_cursor_word_right(struct Input *input) {
    if (input->cursor >= input->str_len) {
        return;
    }
    while (input->cursor < input->str_len && input->str[input->cursor] != ' ') {
        input_move_cursor(input, +1);
    }
    while (input->cursor < input->str_len && input->str[input->cursor] == ' ') {
        input_move_cursor(input, +1);
    }
}

void input_insert(struct Input *input, char c) {
    if (input->cursor < 0 || input->cursor > input->str_len) {
        return;
    }
    // corner case - when fullfilled
    if (input->cursor == input->str_len && input->str_len == STR_CAP) {
        return;
    }
    for (int i = STR_CAP-1; i > input->cursor; --i) {
        input->str[i] = input->str[i-1];
    }
    input->str[input->cursor] = c;
    ++input->str_len;
    if (input->str_len > STR_CAP) {
        input->str_len = STR_CAP;
    }
    input_move_cursor(input, +1);
}

void input_delete(struct Input *input) {
    if (input->cursor < 0 || input->cursor >= input->str_len) {
        return;
    }
    for (int i = input->cursor+1; i < STR_CAP; ++i) {
        input->str[i-1] = input->str[i];
    }
    input->str[STR_CAP-1] = 0;
    --input->str_len;
}

void input_backspace(struct Input *input) {
    if (input->cursor < 1 || input->cursor > input->str_len) {
        return;
    }
    input_move_cursor(input, -1);
    input_delete(input);
}

void input_backspace_word(struct Input *input) {
    if (input->cursor <= 0) {
        return;
    }
    int original_cursor = input->cursor;
    input_move_cursor_word_left(input);
    int chars_to_delete = original_cursor - input->cursor;
    for (int i = 0; i < chars_to_delete; ++i) {
        input_delete(input);
    }
}

void input_delete_word(struct Input *input) {
    if (input->cursor >= input->str_len) {
        return;
    }
    int original_cursor = input->cursor;
    input_move_cursor_word_right(input);
    int chars_to_delete = input->cursor - original_cursor;
    input->cursor = original_cursor;
    for (int i = 0; i < chars_to_delete; ++i) {
        input_delete(input);
    }
}

void input_print(struct Input *input) {
    clear_line();
    printf("> %s", input->str);
    draw_cursor(input->cursor);
}
