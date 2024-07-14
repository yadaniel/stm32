#include "main.h"
#include "random.h"

volatile uint32_t rcc_interrupt_counter;
volatile uint32_t tim7_interrupt_counter;

typedef enum LedState {
    Mode0,
    Mode1,
    Mode2,
} LedState_t;

// arm-none-eabi-objdump.exe --section=.ccmram -d CCMRAM_code.elf

void __attribute__((section(".ccmram_code"))) mode0(uint32_t restart) {
    static uint32_t cnt = 0;

    if(restart) {
        HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LD7_GPIO_Port, LD7_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LD8_GPIO_Port, LD8_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LD9_GPIO_Port, LD9_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LD10_GPIO_Port, LD10_Pin, GPIO_PIN_RESET);
        cnt = 0;
        return;
    }

    cnt += 1;

    switch (cnt % 8) {
        case 0:
            HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
            break;

        case 1:
            HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
            break;

        case 2:
            HAL_GPIO_TogglePin(LD5_GPIO_Port, LD5_Pin);
            break;

        case 3:
            HAL_GPIO_TogglePin(LD6_GPIO_Port, LD6_Pin);
            break;

        case 4:
            HAL_GPIO_TogglePin(LD7_GPIO_Port, LD7_Pin);
            break;

        case 5:
            HAL_GPIO_TogglePin(LD8_GPIO_Port, LD8_Pin);
            break;

        case 6:
            HAL_GPIO_TogglePin(LD9_GPIO_Port, LD9_Pin);
            break;

        case 7:
            HAL_GPIO_TogglePin(LD10_GPIO_Port, LD10_Pin);
            break;
    }
}

void __attribute__((section(".ccmram_code"))) mode1(uint32_t restart) {
    static uint32_t cnt = 0;

    if(restart) {
        HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LD7_GPIO_Port, LD7_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LD8_GPIO_Port, LD8_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LD9_GPIO_Port, LD9_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LD10_GPIO_Port, LD10_Pin, GPIO_PIN_RESET);
        cnt = 0;
        return;
    }
    cnt += 1;

    switch (cnt % 8) {
        case 0:
            HAL_GPIO_TogglePin(LD5_GPIO_Port, LD5_Pin);
            HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
            break;

        case 1:
            HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
            HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
            break;

        case 2:
            HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
            HAL_GPIO_TogglePin(LD6_GPIO_Port, LD6_Pin);
            break;

        case 3:
            HAL_GPIO_TogglePin(LD6_GPIO_Port, LD6_Pin);
            HAL_GPIO_TogglePin(LD8_GPIO_Port, LD8_Pin);
            break;

        case 4:
            HAL_GPIO_TogglePin(LD8_GPIO_Port, LD8_Pin);
            HAL_GPIO_TogglePin(LD10_GPIO_Port, LD10_Pin);
            break;

        case 5:
            HAL_GPIO_TogglePin(LD10_GPIO_Port, LD10_Pin);
            HAL_GPIO_TogglePin(LD9_GPIO_Port, LD9_Pin);
            break;

        case 6:
            HAL_GPIO_TogglePin(LD9_GPIO_Port, LD9_Pin);
            HAL_GPIO_TogglePin(LD7_GPIO_Port, LD7_Pin);
            break;

        case 7:
            HAL_GPIO_TogglePin(LD7_GPIO_Port, LD7_Pin);
            HAL_GPIO_TogglePin(LD5_GPIO_Port, LD5_Pin);
            break;
    }
}

void __attribute__((section(".ccmram_code"))) mode2(uint32_t restart) {

    uint32_t u32 = rand();

    if(u32 & 1)
        HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);

    if(u32 & 2)
        HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);

    if(u32 & 4)
        HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);

    if(u32 & 8)
        HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);

    if(u32 & 16)
        HAL_GPIO_WritePin(LD7_GPIO_Port, LD7_Pin, GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(LD7_GPIO_Port, LD7_Pin, GPIO_PIN_SET);

    if(u32 & 32)
        HAL_GPIO_WritePin(LD8_GPIO_Port, LD8_Pin, GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(LD8_GPIO_Port, LD8_Pin, GPIO_PIN_SET);

    if(u32 & 64)
        HAL_GPIO_WritePin(LD9_GPIO_Port, LD9_Pin, GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(LD9_GPIO_Port, LD9_Pin, GPIO_PIN_SET);

    if(u32 & 128)
        HAL_GPIO_WritePin(LD10_GPIO_Port, LD10_Pin, GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(LD10_GPIO_Port, LD10_Pin, GPIO_PIN_SET);
}

void main_fw(void) {

    // default
    LedState_t ledState = Mode0;
    uint32_t delay_ms = 25;
    uint8_t b, b_prev = 0;

    while (1) {

        HAL_Delay(delay_ms);

        b = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);
        if ((b_prev == 0) && (b == 1)) {

            // HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
            // HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
            // HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
            // HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
            // HAL_GPIO_WritePin(LD7_GPIO_Port, LD7_Pin, GPIO_PIN_RESET);
            // HAL_GPIO_WritePin(LD8_GPIO_Port, LD8_Pin, GPIO_PIN_RESET);
            // HAL_GPIO_WritePin(LD9_GPIO_Port, LD9_Pin, GPIO_PIN_RESET);
            // HAL_GPIO_WritePin(LD10_GPIO_Port, LD10_Pin, GPIO_PIN_RESET);

            switch (ledState) {
                case Mode0:
                    ledState = Mode1;
                    delay_ms = 100;
                    mode1(1);
                    break;

                case Mode1:
                    ledState = Mode2;
                    delay_ms = 25;
                    mode2(1);
                    break;

                case Mode2:
                    ledState = Mode0;
                    delay_ms = 50;
                    mode0(1);
                    srand(SysTick->VAL);
                    break;
            }
        }
        b_prev = b;

        switch (ledState) {
            case Mode0:
                // call test located in CCMRAM
                mode0(0);
                break;

            case Mode1:
                // call test located in CCMRAM
                mode1(0);
                break;

            case Mode2:
                // call test located in CCMRAM
                mode2(0);
                break;
        }
    }
}
