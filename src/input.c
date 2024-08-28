#include "input.h"

void input_init(struct Input* input) {
    memset(input->str, 0, STRING_CAP);
    input->cursor = 0;
    input->len = 0;
}

void input_move_cursor(struct Input *input, int delta) {
    input->cursor += delta;
    if (input->cursor < 0) {
        input->cursor = 0;
    }
    if (input->cursor > input->len) {
        input->cursor = input->len;
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
    if (input->cursor >= input->len) {
        return;
    }
    while (input->cursor < input->len && input->str[input->cursor] != ' ') {
        input_move_cursor(input, +1);
    }
    while (input->cursor < input->len && input->str[input->cursor] == ' ') {
        input_move_cursor(input, +1);
    }
}

void input_insert(struct Input *input, char c) {
    if (input->cursor < 0 || input->cursor > input->len) {
        return;
    }
    // corner case - when fullfilled
    if (input->cursor == input->len && input->len == STRING_CAP) {
        return;
    }
    for (int i = STRING_CAP-1; i > input->cursor; --i) {
        input->str[i] = input->str[i-1];
    }
    input->str[input->cursor] = c;
    ++input->len;
    if (input->len > STRING_CAP) {
        input->len = STRING_CAP;
    }
    input_move_cursor(input, +1);
}

void input_delete(struct Input *input) {
    if (input->cursor < 0 || input->cursor >= input->len) {
        return;
    }
    for (int i = input->cursor+1; i < STRING_CAP; ++i) {
        input->str[i-1] = input->str[i];
    }
    input->str[STRING_CAP-1] = 0;
    --input->len;
}

void input_backspace(struct Input *input) {
    if (input->cursor < 1 || input->cursor > input->len) {
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
    if (input->cursor >= input->len) {
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
    int prompt_size = print_prompt_line();
    printf("%s", input->str);
    draw_cursor(input->cursor, prompt_size+1);
}

void history_append(struct History* history, struct Input* input) {
    if (history->len >= HISTORY_CAP) {
        return;
    }
    history->inputs[history->len] = *input;
    ++history->len;
}

void history_init(struct History* history) {
    history->current = history->len = 0;

    int lines = count_lines_file(HISTORY_FILE);

    FILE* file = fopen(HISTORY_FILE, "r");
    if (file == NULL) {
        struct Input input;
        input_init(&input);
        history_append(history, &input);
        history->current = history->len-1;
        return;
    }

    char* str = NULL;
    size_t len = 0;
    int lines_read = 0;

    struct Input input;
    while (read_line(&str, &len, file) != -1) {
        if (lines_read < lines-(HISTORY_CAP-1)) {
            ++lines_read;
            continue;
        }

        str[strcspn(str, "\n")] = 0;

        input_init(&input);
        strcpy(input.str, str);
        input.cursor = input.len = strlen(input.str);
        history_append(history, &input);
    }

    input_init(&input);
    history_append(history, &input);
    history->current = history->len-1;

    free(str);
    fclose(file);
}
