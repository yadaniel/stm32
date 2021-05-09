#include "uart2_printf.h"
#include "crc_flash.h"
/* #include "../Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal.h" */
#include "../Drivers/CMSIS/Device/ST/STM32L4xx/Include/stm32l4xx.h"

#define FLASH_ADDR_SAVED_CRC    0x0803EFFC

typedef struct crc_t {
    uint32_t saved;
    uint32_t calculated;
} crc_t;

crc_t crc_data = {.saved = 0x00, .calculated = 0x00};

// d = ba.a2b_hex("00112233445566778899AABBCCDDEEFF0123456789ABCDEF")
// c = crc.crc.Crc32Bzip2()
// hex(c.process(d).final() ^ 0xFFFFFFFF) # => '0xc4e0c81'
// python result is XORed with 0xFFFFFFFF
// stm32 below is not XORed but has same result
uint8_t test_crc(void) {
    CRC->INIT = 0xFFFFFFFF;     // seed
    CRC->POL = 0x04C11DB7;      // CCITT32
    CRC->CR = 1;
    for(uint32_t p = 0x08000000; p<FLASH_ADDR_SAVED_CRC; p+=4) {
        CRC->DR = *((uint32_t *)p);
    }
    uint32_t crc = CRC->DR;
    /* crc ^= 0xFFFFFFFF; */
    /* crc ^= 0x00000000; */
    uint32_t stored_crc = (*(uint32_t *)FLASH_ADDR_SAVED_CRC);

    crc_data.saved = stored_crc;
    crc_data.calculated = crc;

    uint8_t result = 0;
    if (crc == stored_crc) {
        result = 1;
    }
    return result;
}

void crc_print(void) {
    uint8_t crc_result = test_crc();
    uint8_t msg[] = "crc passed = ";

    for(int8_t idx=0; idx<sizeof(msg)/sizeof(msg[0])-1; idx+=1) {
        UART2_Sendchar(msg[idx]);
    }
    UART2_Sendchar(crc_result + '0');
    UART2_Sendchar(0x0D);

    UART2_print_uint32(crc_data.saved, "crc save = ");
    UART2_print_uint32(crc_data.calculated, "crc calc = ");
}

