#ifndef SHELL_INCLUDED
#define SHELL_INCLUDED

#include <stdint.h>

#define IDLE    0
#define CMD1    1
#define CMD2    2
#define CMD3    3

uint8_t shell_run(void);

#endif  // SHELL_INCLUDED

