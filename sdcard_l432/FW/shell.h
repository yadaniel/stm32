#ifndef SHELL_INCLUDED
#define SHELL_INCLUDED

#include <stdint.h>

#define ERR     0
#define IDLE    1
#define CMD1    2   // f_mount
#define CMD2    3   // f_open
#define CMD3    4   // f_read
#define CMD4    5   // add x y
#define CMD5    6   // reset
#define CMD6    7   // time

void shell_run(void);

#endif  // SHELL_INCLUDED

