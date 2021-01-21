#ifndef SHELL_INCLUDED
#define SHELL_INCLUDED

#include <stdint.h>

#define ERR     0
#define IDLE    1
#define CMD1    2
#define CMD2    3
#define CMD3    4
#define CMD4    5

void shell_run(void);

#endif  // SHELL_INCLUDED

