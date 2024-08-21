#include <ctype.h>
#include <termios.h>


#define SEQ_CAP 8

enum KeyAction {
    KEY_NONE,
    KEY_NEWLINE,
    KEY_NORMAL,

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

void termios_enter(struct termios *old_term);
void termios_leave(struct termios *old_term);

enum KeyAction identify_key(const char *sequence);
int read_sequence(char *buffer);
void get_cmd();
