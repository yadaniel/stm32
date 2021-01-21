#include "shell.h"
#include "main_fw.h"
#include "../Core/Inc/main.h"

extern volatile uint8_t rx_buffer[32];
extern volatile uint8_t rx_idx;
extern volatile uint8_t rx_done;

extern volatile uint8_t tx_buffer[32];
extern volatile uint8_t tx_idx;
extern volatile uint8_t tx_stop;

uint8_t shell_state = CMD1;

void shell_run(void) {

    if(rx_done == 1) {
        if((rx_buffer[0] == 'C') && (rx_buffer[1] == 'M') && (rx_buffer[2] == 'D')) {
            switch(rx_buffer[3]) {
                case '1':
                    shell_state = CMD1;
                    break;
                case '2':
                    shell_state = CMD2;
                    break;
                case '3':
                    shell_state = CMD3;
                    break;
                default:
                    shell_state = IDLE;
                    break;
            }
        }
        __disable_irq();
        rx_done = 0;
        __enable_irq();
    }

    switch(shell_state) {
        case IDLE:
            break;

        case CMD1:
            shell_state = IDLE;
            __disable_irq();
            tx_idx = 0;
            tx_stop = 0;
            tx_buffer[tx_stop++] = 'C';
            tx_buffer[tx_stop++] = 'M';
            tx_buffer[tx_stop++] = 'D';
            tx_buffer[tx_stop++] = '1';
            tx_buffer[tx_stop++] = 0x0D;
            __enable_irq();
            USART2->TDR = 0;
            break;

        case CMD2:
            shell_state = IDLE;
            __disable_irq();
            tx_idx = 0;
            tx_stop = 0;
            tx_buffer[tx_stop++] = 'C';
            tx_buffer[tx_stop++] = 'M';
            tx_buffer[tx_stop++] = 'D';
            tx_buffer[tx_stop++] = '2';
            tx_buffer[tx_stop++] = 0x0D;
            __enable_irq();
            USART2->TDR = 0;
            break;

        case CMD3:
            shell_state = IDLE;
            __disable_irq();
            tx_idx = 0;
            tx_stop = 0;
            tx_buffer[tx_stop++] = 'C';
            tx_buffer[tx_stop++] = 'M';
            tx_buffer[tx_stop++] = 'D';
            tx_buffer[tx_stop++] = '3';
            tx_buffer[tx_stop++] = 0x0D;
            __enable_irq();
            USART2->TDR = 0;
            break;

        default:
            shell_state = IDLE;
            break;
    }
}

