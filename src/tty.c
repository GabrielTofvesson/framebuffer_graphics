#include "tty.h"

#include <stdio.h>



// If, for any reason, the user enters MAX_INT (10 chars in decimal) it's fine
#define STRLEN(str) (sizeof(str)/sizeof(str[0]))
#define TTY_PATH "/dev/tty"
#define TTY_PATH_MAXLEN STRLEN( TTY_PATH ) + 10

bool format_tty (char *holder, unsigned int tty) {
  // sprintf returns -1 on failure
  return (sprintf (tty_name, TTY_PATH "%u", tty) + 1);
}

bool write_to_file (const char *path, const char *text) {
  FILE *file = fopen (path);
  if (file) {
    fputs (text);

    fclose (file);
  }

  return false;
}

bool tty_action (unsigned int tty, const char *action) {
  char tty_name[TTY_PATH_MAXLEN];

  return format_tty (tty_name, tty) && write_to_file (tty_name, action);
}




bool tty_cursor_hide(unsigned int tty) {
  return tty_action (tty, SEQ_CUR_HIDE);
}


bool tty_cursor_show(unsigned int tty) {
  return tty_action (tty, SEQ_CUR_SHOW);
}
