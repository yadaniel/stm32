#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include "main_fw.h"
#include "bsec_integration.h"
#include "bsec_user.h"
#include "../Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal.h"
#include "../Drivers/CMSIS/Device/ST/STM32L4xx/Include/stm32l432xx.h"
/* #include "../Drivers/CMSIS/Device/ST/STM32L4xx/Include/stm32l3xx.h" */

void UART_Sendchar(uint8_t c) {
    while(!(USART2->ISR & USART_ISR_TXE)); // block until tx empty
    USART2->TDR = c;
}

uint8_t UART_Getchar() {
    uint8_t c;
    while(!(USART2->ISR & USART_ISR_RXNE));  // nothing received so just block
    c = USART2->RDR; // read Receiver buffer register
    return c;
}

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
    for(uint32_t p = 0x08000000; p<0x0803FFFC; p+=4) {
        CRC->DR = *((uint32_t *)p);
    }
    uint32_t crc = CRC->DR;
    /* crc ^= 0xFFFFFFFF; */
    /* crc ^= 0x00000000; */
    uint32_t stored_crc = (*(uint32_t *)0x0803FFFC);

    crc_data.saved = stored_crc;
    crc_data.calculated = crc;

    uint8_t result = 0;
    if (crc == stored_crc) {
        result = 1;
    }
    return result;
}

void print_uint32(uint32_t u32, const char * msg) {
    const uint8_t tbl[] = "0123456789ABCDEF";

    while(msg && *msg) {
        UART_Sendchar(*msg);
        msg += 1;
    }
    uint8_t d3 = ((u32 >> 24) & 0xFF);
    uint8_t d2 = ((u32 >> 16) & 0xFF);
    uint8_t d1 = ((u32 >> 8) & 0xFF);
    uint8_t d0 = ((u32 >> 0) & 0xFF);

    UART_Sendchar(tbl[(d3 >> 4) & 0x0F]);
    UART_Sendchar(tbl[(d3 >> 0) & 0x0F]);
    UART_Sendchar(tbl[(d2 >> 4) & 0x0F]);
    UART_Sendchar(tbl[(d2 >> 0) & 0x0F]);
    UART_Sendchar(tbl[(d1 >> 4) & 0x0F]);
    UART_Sendchar(tbl[(d1 >> 0) & 0x0F]);
    UART_Sendchar(tbl[(d0 >> 4) & 0x0F]);
    UART_Sendchar(tbl[(d0 >> 0) & 0x0F]);

    UART_Sendchar(0x0D);
}

void crc_print(void) {
    uint8_t crc_result = test_crc();
    uint8_t msg[] = "crc passed = ";

    for(int8_t idx=0; idx<sizeof(msg)/sizeof(msg[0])-1; idx+=1) {
        UART_Sendchar(msg[idx]);
    }
    UART_Sendchar(crc_result + '0');
    UART_Sendchar(0x0D);

    print_uint32(crc_data.saved, "crc save = ");
    print_uint32(crc_data.calculated, "crc calc = ");
}


/* The GCC C library makes calls to the following functions to perform low-level I/O : */
/* int _read(int file, char *data, int len) */
/* int _write(int file, char *data, int len) */
/* int _close(int file) */
/* int _lseek(int file, int ptr, int dir) */
/* int _fstat(int file, struct stat *st) */
/* int _isatty(int file) */

void vprint(const char *fmt, va_list argp) {
    char msg[128] = {0};
    const uint8_t MSG_LEN = sizeof(msg)/sizeof(msg[0]);
    if(0 < vsnprintf(&msg[0],MSG_LEN, fmt,argp)) {
        const uint8_t msg_len = strlen(msg);
        for(uint8_t i=0; i<msg_len; i+=1) {
            UART_Sendchar(msg[i]);
        }
        // UART_Sendchar(0x0D);
    }
}

void my_printf(const char *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    vprint(fmt, argp);
    va_end(argp);
}

void bme_exit(int8_t err) {
    while(1) {
        HAL_Delay(1000);
        switch(err) {
            case BME680_E_NULL_PTR:
                my_printf("null ptr error\n");
                break;
            case BME680_E_COM_FAIL:
                my_printf("communication failed error\n");
                break;
            case BME680_E_DEV_NOT_FOUND:
                my_printf("device not found error\n");
                break;
            case BME680_E_INVALID_LENGTH:
                my_printf("invalid length error\n");
                break;
            case BME680_E_NOTOK:
                my_printf("not ok error\n");
                break;
            default:
                my_printf("unknown error\n");
                break;
        }
    }
}

int main_fw(void) {
    crc_print();

    // test my_printf
    //my_printf("data = %.2f, foo = 0x%04X", 0.1234, 0x1234);

    struct bme680_dev gas_sensor;
    gas_sensor.dev_id = 0x76;   // SDO pulled low
    // gas_sensor.dev_id = BME680_I2C_ADDR_PRIMARY;     // 0x76 SDO pulled low
    gas_sensor.intf = BME680_I2C_INTF;
    gas_sensor.read = user_i2c_read;
    gas_sensor.write = user_i2c_write;
    gas_sensor.delay_ms = user_delay_ms;
    /* amb_temp can be set to 25 prior to configuring the gas sensor
     * or by performing a few temperature readings without operating the gas sensor.
     */
    gas_sensor.amb_temp = 25;

    int8_t rslt = BME680_OK;
    rslt = bme680_init(&gas_sensor);
    if(rslt == BME680_OK) {
        my_printf("BME680 initialized\n");
    } else {
        bme_exit(rslt);
    }

    uint8_t set_required_settings;
    /* Set the temperature, pressure and humidity settings */
    gas_sensor.tph_sett.os_hum = BME680_OS_2X;
    gas_sensor.tph_sett.os_pres = BME680_OS_4X;
    gas_sensor.tph_sett.os_temp = BME680_OS_8X;
    gas_sensor.tph_sett.filter = BME680_FILTER_SIZE_3;

    /* Set the remaining gas sensor settings and link the heating profile */
    gas_sensor.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
    /* Create a ramp heat waveform in 3 steps */
    gas_sensor.gas_sett.heatr_temp = 320; /* degree Celsius */
    gas_sensor.gas_sett.heatr_dur = 150; /* milliseconds */

    /* Select the power mode */
    /* Must be set before writing the sensor configuration */
    gas_sensor.power_mode = BME680_FORCED_MODE;

    /* Set the required sensor settings needed */
    set_required_settings = BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL | BME680_FILTER_SEL | BME680_GAS_SENSOR_SEL;

    /* Set the desired sensor configuration */
    rslt = bme680_set_sensor_settings(set_required_settings, &gas_sensor);
    if(rslt == BME680_OK) {
        my_printf("BME680 settings done\n");
    } else {
        bme_exit(rslt);
    }

    /* Set the power mode */
    rslt = bme680_set_sensor_mode(&gas_sensor);
    if(rslt == BME680_OK) {
        my_printf("BME680 sensor mode set\n");
    } else {
        bme_exit(rslt);
    }

    /* Get the total measurement duration so as to sleep or wait till the
     * measurement is complete */
    uint16_t meas_period = 0;
    bme680_get_profile_dur(&meas_period, &gas_sensor);

    while(1) {
        user_delay_ms(meas_period); /* Delay till the measurement is ready */

        struct bme680_field_data data = {0};
        rslt = bme680_get_sensor_data(&data, &gas_sensor);
        if(rslt == BME680_OK) {
            my_printf("BME680 read ... ");
        } else {
            bme_exit(rslt);
        }

        my_printf("T: %.2f degC, P: %.2f hPa, H: %.2f %%rH", data.temperature / 100.0f, data.pressure / 100.0f, data.humidity / 1000.0f );
        /* Avoid using measurements from an unstable heating setup */
        if(data.status & BME680_GASM_VALID_MSK) {
            my_printf(", G: %d ohms", data.gas_resistance);
            my_printf(" ... %s", iaq_text(data.gas_resistance));
        }
        my_printf("\r\n");

        HAL_Delay(500);

        /* Trigger the next measurement if you would like to read data out continuously */
        if (gas_sensor.power_mode == BME680_FORCED_MODE) {
            rslt = bme680_set_sensor_mode(&gas_sensor);
            if(rslt != BME680_OK) {
                bme_exit(rslt);
            }
        }
    }

    // while(1) {
    //     uint8_t c = UART_Getchar();
    //     UART_Sendchar(c);
    //     if(c=='r') {
    //         NVIC_SystemReset();
    //     }
    // }

    return 0;
}

