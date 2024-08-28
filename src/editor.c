#include "editor.h"


void termios_enter(struct termios* old_term) {
    tcgetattr(STDIN_FILENO, old_term);

    struct termios new_term = *old_term;
    new_term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);

    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    set_cursor_style(6);
}

void termios_leave(struct termios* old_term) {
    set_cursor_style(1);
    tcsetattr(STDIN_FILENO, TCSANOW, old_term);
}

enum KeyAction identify_key(const char* sequence) {
    if (strcmp(sequence, "\x7f") == 0) {
        return KEY_BACKSPACE;
    } else if (strcmp(sequence, "\033[3~") == 0) {
        return KEY_DELETE;
    } else if (strcmp(sequence, "\033[A") == 0) {
        return KEY_ARROW_UP;
    } else if (strcmp(sequence, "\033[B") == 0) {
        return KEY_ARROW_DOWN;
    } else if (strcmp(sequence, "\033[C") == 0) {
        return KEY_ARROW_RIGHT;
    } else if (strcmp(sequence, "\033[D") == 0) {
        return KEY_ARROW_LEFT;
    } else if (strcmp(sequence, "\033[1;5C") == 0) {
        return KEY_CTRL_ARROW_RIGHT;
    } else if (strcmp(sequence, "\033[1;5D") == 0) {
        return KEY_CTRL_ARROW_LEFT;
    } else if (strcmp(sequence, "\033[3;5~") == 0) {
        return KEY_CTRL_DELETE;
    } else if (strcmp(sequence, "\x17") == 0) {
        return KEY_CTRL_BACKSPACE;
    } else if (strcmp(sequence, "\n") == 0) {
        return KEY_NEWLINE;
    } else if (strlen(sequence) == 1 && isprint(sequence[0])) {
        return KEY_PRINTABLE;
    }
    return KEY_NONE;
}

ssize_t get_cmd(char** cmd, size_t* maxlen) {
    if (!isatty(STDIN_FILENO)) {
        // used for tests
        setbuf(stdin, NULL);
        fprintf(stderr, "> ");
        return read_line(cmd, maxlen, stdin);
    }

    struct termios old_term;
    termios_enter(&old_term);

    struct History history;
    history_init(&history);

    input_print(&history.inputs[history.current]);

    char sequence[SEQUENCE_CAP];
    enum KeyAction action = KEY_NONE;
    while (action != KEY_NEWLINE) {
        read_sequence(sequence);
        action = identify_key(sequence);

        switch (action) {
            case KEY_BACKSPACE:
                input_backspace(&history.inputs[history.current]);
                break;
            case KEY_DELETE:
                input_delete(&history.inputs[history.current]);
                break;
            case KEY_ARROW_LEFT:
                input_move_cursor(&history.inputs[history.current], -1);
                break;
            case KEY_ARROW_RIGHT:
                input_move_cursor(&history.inputs[history.current], +1);
                break;
            case KEY_ARROW_UP:
                if (history.current-1 >= 0) {
                    --history.current;
                }
                break;
            case KEY_ARROW_DOWN:
                if (history.current+1 < history.len) {
                    ++history.current;
                }
                break;

            case KEY_CTRL_ARROW_LEFT:
                input_move_cursor_word_left(&history.inputs[history.current]);
                break;
            case KEY_CTRL_ARROW_RIGHT:
                input_move_cursor_word_right(&history.inputs[history.current]);
                break;
            case KEY_CTRL_BACKSPACE:
                input_backspace_word(&history.inputs[history.current]);
                break;
            case KEY_CTRL_DELETE:
                input_delete_word(&history.inputs[history.current]);
                break;

            case KEY_PRINTABLE:
                input_insert(&history.inputs[history.current], sequence[0]);
                break;
            default:
                break;
        }
        input_print(&history.inputs[history.current]);
    }

    termios_leave(&old_term);

    if (history.inputs[history.current].len > 0) {
        append_to_file(HISTORY_FILE, history.inputs[history.current].str);
    }

    // newline for cmd execution
    printf("\n");

    // use strlen() instead of input.len just to be fully safe
    *cmd = realloc_panic(*cmd, strlen(history.inputs[history.current].str)+1);
    strcpy(*cmd, history.inputs[history.current].str);
    return strlen(*cmd);
}
