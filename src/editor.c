#include "editor.h"
#include "input.h"


void termios_enter(struct termios *old_term) {
    tcgetattr(STDIN_FILENO, old_term);

    struct termios new_term = *old_term;
    new_term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);

    setbuf(stdout, NULL);
    set_cursor_style(6);
}

void termios_leave(struct termios *old_term) {
    set_cursor_style(1);
    tcsetattr(STDIN_FILENO, TCSANOW, old_term);
}

enum KeyAction identify_key(const char *sequence) {
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
        return KEY_NORMAL;
    }
    return KEY_NONE;
}

int read_sequence(char *buffer) {
    memset(buffer, 0, SEQ_CAP);
    int nread = 0;
    while ((nread = read_panic(STDIN_FILENO, buffer, 1)) == 0);

    if (buffer[0] == '\033') {
        nread += read(STDIN_FILENO, buffer+1, SEQ_CAP-1);
    }
    return nread;
}

void get_cmd() {
    struct termios old_term;
    termios_enter(&old_term);

    struct Input input;
    input_init(&input);

    input_print(&input);

    char sequence[SEQ_CAP];
    enum KeyAction action = KEY_NONE;
    while (action != KEY_NEWLINE) {
        read_sequence(sequence);
        action = identify_key(sequence);

        switch (action) {
            case KEY_BACKSPACE:
                input_backspace(&input);
                break;
            case KEY_DELETE:
                input_delete(&input);
                break;
            case KEY_ARROW_LEFT:
                input_move_cursor(&input, -1);
                break;
            case KEY_ARROW_RIGHT:
                input_move_cursor(&input, +1);
                break;
            case KEY_ARROW_UP:
            case KEY_ARROW_DOWN:
                // TODO: add support for cmd history
                break;

            case KEY_CTRL_ARROW_LEFT:
                input_move_cursor_word_left(&input);
                break;
            case KEY_CTRL_ARROW_RIGHT:
                input_move_cursor_word_right(&input);
                break;
            case KEY_CTRL_BACKSPACE:
                input_backspace_word(&input);
                break;
            case KEY_CTRL_DELETE:
                input_delete_word(&input);
                break;

            case KEY_NORMAL:
                input_insert(&input, sequence[0]);
                break;
            default:
                break;
        }
        input_print(&input);
    }

    termios_leave(&old_term);
    // printf("\n##%s##", input.str);
}
