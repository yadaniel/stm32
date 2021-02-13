#ifndef SPIBUS_INCLUDED
#define SPIBUS_INCLUDED

#include <stdint.h>

// hardware spi lines
#define SPIBUS_CLK      LL_GPIO_PIN_5   // PA5
#define SPIBUS_MISO     LL_GPIO_PIN_6   // PA6
#define SPIBUS_MOSI     LL_GPIO_PIN_7   // PA7
#define SPIBUS_CS_OUT   LL_GPIO_PIN_0   // PB0

// module spibus public API
void spibus_init_soft(void);
void spibus_init_hard_ad7918(void);
uint16_t spibus_send_recv16_ad7918_soft(uint16_t dataOut);
uint16_t spibus_send_recv16_ad7918_hard(uint16_t dataOut);
uint16_t spibus_send_recv16(uint16_t data);

#endif  /* SPIBUS_INCLUDED */

