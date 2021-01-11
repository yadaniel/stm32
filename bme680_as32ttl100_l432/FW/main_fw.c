#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include "main_fw.h"
#include "crc_flash.h"
#include "bsec_integration.h"
#include "bsec_user.h"
#include "uart1_printf.h"
#include "uart2_printf.h"
#include "../Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal.h"
#include "../Drivers/CMSIS/Device/ST/STM32L4xx/Include/stm32l432xx.h"
/* #include "../Drivers/CMSIS/Device/ST/STM32L4xx/Include/stm32l4xx.h" */

int main_fw(void) {
    crc_print();

    // test UART2_printf
    //UART2_printf("data = %.2f, foo = 0x%04X", 0.1234, 0x1234);

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
        UART2_printf("BME680 initialized\n");
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
        UART2_printf("BME680 settings done\n");
    } else {
        bme_exit(rslt);
    }

    /* Set the power mode */
    rslt = bme680_set_sensor_mode(&gas_sensor);
    if(rslt == BME680_OK) {
        UART2_printf("BME680 sensor mode set\n");
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
            UART2_printf("BME680 read ... ");
        } else {
            bme_exit(rslt);
        }

        UART1_printf("T: %.2f degC, P: %.2f hPa, H: %.2f %%rH", data.temperature / 100.0f, data.pressure / 100.0f, data.humidity / 1000.0f );
        UART2_printf("T: %.2f degC, P: %.2f hPa, H: %.2f %%rH", data.temperature / 100.0f, data.pressure / 100.0f, data.humidity / 1000.0f );
        /* Avoid using measurements from an unstable heating setup */
        if(data.status & BME680_GASM_VALID_MSK) {
            UART1_printf(", G: %d ohms", data.gas_resistance);
            UART2_printf(", G: %d ohms", data.gas_resistance);
            UART2_printf(" ... %s", iaq_text(data.gas_resistance));
        }
        UART1_printf("\r\n");
        UART2_printf("\r\n");

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
    //     UART2_Sendchar(c);
    //     if(c=='r') {
    //         NVIC_SystemReset();
    //     }
    // }

    return 0;
}

