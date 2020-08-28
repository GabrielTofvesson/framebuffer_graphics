#ifndef TTY_H
#define TTY_H

// ANSI escape char
#define ESC "\e"

// ANSI Control Sequence Introducer
#define CSI ESC "["

#define SEQ_CUR_HIDE CSI "?25l"
#define SEQ_CUR_SHOW CSI "?25h"


/**
 * Hide cursor for a given tty device
 */
bool tty_cursor_hide(unsigned int tty);

/**
 * Show cursor for a given tty device
 */
bool tty_cursor_show(unsigned int tty);

#endif
