#ifndef INC_RANDOM_H_
#define INC_RANDOM_H_

#include <stdint.h>

extern uint32_t random_next;

uint32_t rand(void);
void srand(uint32_t seed);

#endif /* INC_RANDOM_H_ */
