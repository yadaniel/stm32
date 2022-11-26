#include "max31855.h"

uint8_t max31855_error = 0;
uint8_t DATARX[4];  // raw data

float max31855_read_temp(void) {
    uint32_t temp = 0;
    uint8_t sign;

    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Receive(&hspi3, DATARX, 4, 1000);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

    max31855_error = DATARX[3] & 0x07; 	// error detection
    sign = (DATARX[0] & (0x80)) >> 7; 	// sign bit calculation

    if(DATARX[3] & 0x07) {
    	// returns error number
    	return -1.0f * (DATARX[3] & 0x07);
    }
    else if(sign == 1) {
    	// negative temperature
        temp = (DATARX[0] << 6) | (DATARX[1] >> 2);
        temp &= 0b01111111111111;
        temp ^= 0b01111111111111;
        return -temp/4.0f;
    } else {
    	// positive temperature
        temp = (DATARX[0] << 6) | (DATARX[1] >> 2);
        return temp/4.0f;
    }
}
