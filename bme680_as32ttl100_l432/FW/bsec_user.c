#include "../Core/Inc/i2c.h"
#include "../Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal.h"
#include "../Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal_i2c.h"
#include "../Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal_i2c_ex.h"
#include "bsec_integration.h"
#include "uart2_printf.h"
#include "bsec_user.h"

int8_t user_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len) {
    int8_t rslt = BME680_OK; /* Return 0 for Success, non-zero for failure */

    /*
     * Data on the bus should be like
     * |------------+---------------------|
     * | I2C action | Data                |
     * |------------+---------------------|
     * | Start      | -                   |
     * | Write      | (reg_addr)          |
     * | Stop       | -                   |
     * | Start      | -                   |
     * | Read       | (reg_data[0])       |
     * | Read       | (....)              |
     * | Read       | (reg_data[len - 1]) |
     * | Stop       | -                   |
     * |------------+---------------------|
     */

    if(HAL_I2C_Master_Transmit(&hi2c3, dev_id << 1, &reg_addr, 1, 100) != HAL_OK) {
        return BME680_E_NOTOK;
    }
    if(HAL_I2C_Master_Receive(&hi2c3, dev_id << 1, reg_data, len, 100) != HAL_OK) {
        return BME680_E_NOTOK;
    }

    return rslt;
}

int8_t user_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len) {
    int8_t rslt = BME680_OK; /* Return 0 for Success, non-zero for failure */

    /*
     * Data on the bus should be like
     * |------------+---------------------|
     * | I2C action | Data                |
     * |------------+---------------------|
     * | Start      | -                   |
     * | Write      | (reg_addr)          |
     * | Write      | (reg_data[0])       |
     * | Write      | (....)              |
     * | Write      | (reg_data[len - 1]) |
     * | Stop       | -                   |
     * |------------+---------------------|
     */

    if(len >= 16) {
        return BME680_E_NOTOK;
    }

    uint8_t buffer[16] = {reg_addr};
    for(uint8_t i=0; i<len; i+=1) {
        buffer[i+1] = reg_data[i];
    }

    if(HAL_I2C_Master_Transmit(&hi2c3, dev_id << 1, &buffer[0], len+1, 100) != HAL_OK) {
        return BME680_E_NOTOK;
    }

    return rslt;
}

void user_delay_ms(uint32_t period) {
    /*
     * Return control or wait,
     * for a period amount of milliseconds
     */

    HAL_Delay(period);
}

const char * iaq_text(uint32_t res) {
    if(res > IAQ_THRES_0) {
        return "very good ++";
    } else if(res > IAQ_THRES_1) {
        return "very good +";
    } else if(res > IAQ_THRES_2) {
        return "very good";
    } else if(res > IAQ_THRES_3) {
        return "good ++";
    } else if(res > IAQ_THRES_4) {
        return "good +";
    } else if(res > IAQ_THRES_5) {
        return "good";
    } else if(res > IAQ_THRES_6) {
        return "average";
    } else if(res > IAQ_THRES_7) {
        return "bad";
    } else if(res > IAQ_THRES_8) {
        return "bad -";
    } else if(res > IAQ_THRES_9) {
        return "bad --";
    } else if(res > IAQ_THRES_10) {
        return "very bad";
    } else if(res > IAQ_THRES_11) {
        return "very bad -";
    } else if(res > IAQ_THRES_12) {
        return "very bad --";
    } else if(res > IAQ_THRES_13) {
        return "critical";
    } else {
        return "emergency";
    }
}

void bme_exit(int8_t err) {
    while(1) {
        HAL_Delay(1000);
        switch(err) {
            case BME680_E_NULL_PTR:
                UART2_printf("null ptr error\n");
                break;
            case BME680_E_COM_FAIL:
                UART2_printf("communication failed error\n");
                break;
            case BME680_E_DEV_NOT_FOUND:
                UART2_printf("device not found error\n");
                break;
            case BME680_E_INVALID_LENGTH:
                UART2_printf("invalid length error\n");
                break;
            case BME680_E_NOTOK:
                UART2_printf("not ok error\n");
                break;
            default:
                UART2_printf("unknown error\n");
                break;
        }
    }
}

