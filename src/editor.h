#ifndef EDITOR_H
#define EDITOR_H

#include "input.h"
#include "utils.h"

#include <ctype.h>
#include <string.h>
#include <termios.h>


enum KeyAction {
    KEY_NONE,
    KEY_NEWLINE,
    KEY_PRINTABLE,

    KEY_BACKSPACE,
    KEY_DELETE,
    KEY_ARROW_UP,
    KEY_ARROW_DOWN,
    KEY_ARROW_LEFT,
    KEY_ARROW_RIGHT,

    KEY_CTRL_DELETE,    
    KEY_CTRL_BACKSPACE,
    KEY_CTRL_ARROW_LEFT,
    KEY_CTRL_ARROW_RIGHT,
};

void termios_enter(struct termios* old_term);
void termios_leave(struct termios* old_term);

enum KeyAction identify_key(const char *sequence);
ssize_t get_cmd(char** cmd, size_t* maxlen);

#endif /* EDITOR_H */
