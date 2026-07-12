#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

extern "C" {
    void init_keyboard();
    void keyboard_handler();

    /*
       Expose a global function to check if the user hit Enter
       and read out the text string they typed.
    */
    char* get_shell_command();
    void  clear_shell_command();
}

#endif
