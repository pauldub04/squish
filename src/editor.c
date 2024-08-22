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

ssize_t read_sequence(char* buffer) {
    memset(buffer, 0, SEQ_CAP);
    ssize_t nread = 0;
    while ((nread = read_panic(STDIN_FILENO, buffer, 1)) == 0);

    if (buffer[0] == '\033') {
        nread += read(STDIN_FILENO, buffer+1, SEQ_CAP-1);
    }
    return nread;
}

ssize_t read_line(char** lineptr, size_t* n, FILE* stream) {
    if (lineptr == NULL || n == NULL || stream == NULL) {
        return -1;
    }

    if (*lineptr == NULL) {
        *lineptr = malloc_panic(STR_CAP);
        *n = STR_CAP;
    }

    size_t pos = 0;
    int c;

    while ((c = fgetc(stream)) != EOF) {
        if (pos >= *n - 1) {
            size_t new_size = *n * 2;
            char *new_lineptr = realloc_panic(*lineptr, new_size);
            *lineptr = new_lineptr;
            *n = new_size;
        }

        (*lineptr)[pos++] = c;
        if (c == '\n') {
            break;
        }
    }

    if (pos == 0 && c == EOF) {
        return -1;
    }

    (*lineptr)[pos] = '\0';
    return pos;
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

            case KEY_PRINTABLE:
                input_insert(&input, sequence[0]);
                break;
            default:
                break;
        }
        input_print(&input);
    }

    termios_leave(&old_term);

    // newline for cmd execution
    printf("\n");

    // use strlen() instead of input.str_len just to be fully safe
    *cmd = realloc_panic(*cmd, strlen(input.str)+1);
    strcpy(*cmd, input.str);
    return strlen(*cmd);
}
