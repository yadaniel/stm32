#ifndef MAX31855_INCLUDED
#define MAX31855_INCLUDED

// adjust module spi interface as needed
#include "spi.h"
extern SPI_HandleTypeDef 	hspi3;

// adjust module gpio interface as needed
#include "gpio.h"
#define CS_GPIO_Port 		SPI3_CS_GPIO_Port
#define CS_Pin 				SPI3_CS_Pin

#include <stdint.h>

// Error Detection
// 1-> No Connection
// 2-> Short to GND
// 4-> Short to VCC
extern uint8_t max31855_error;

float max31855_read_temp(void);

#endif 	// MAX31855_INCLUDED
