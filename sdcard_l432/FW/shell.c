#include "shell.h"
#include "fatfs.h"
#include "main_fw.h"
#include "../Core/Inc/main.h"

extern volatile uint8_t rx_buffer[32];
extern volatile uint8_t rx_idx;
extern volatile uint8_t rx_done;

extern volatile uint8_t tx_buffer[32];
extern volatile uint8_t tx_idx;
extern volatile uint8_t tx_stop;

#define ARRSIZE(arr)    (sizeof(arr)/sizeof(arr[0]))
const uint8_t tbl[] = "0123456789ABCDEF";

uint8_t shell_state = IDLE;
uint8_t shell_err = 0;
uint32_t shell_in1 = 0;
uint32_t shell_in2 = 0;
uint32_t shell_out1 = 0;
uint32_t shell_out2 = 0;

FRESULT fres;
FATFS fs = {0};
FIL f = {0};
FILINFO fi = {0};
char line[64] = {0};
uint8_t line_cnt = 0;

void shell_run(void) {
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t k = 0;
    uint8_t l = 0;
    uint32_t dec = 1;
    uint8_t ok = 0;

    // check if command received
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

            case '4':
                ok = 1;
                //
                i = 4;
                while(i < ARRSIZE(rx_buffer) && rx_buffer[i] == ' ') {
                    i += 1;
                }
                j = i;  // i points to first char of the number1
                while(j < ARRSIZE(rx_buffer) && rx_buffer[j] != ' ') {
                    j += 1;
                }
                k = j;  // j points to past last char of the number1
                while(k < ARRSIZE(rx_buffer) && rx_buffer[k] == ' ') {
                    k += 1;
                }
                l = k;  // k points to first char of the number2
                while(l < ARRSIZE(rx_buffer) && (rx_buffer[l] != 0x0D) && (rx_buffer[l] != ' ')) {
                    l += 1;
                }
                if((i == j) || (k == l) || (j == k) || (rx_buffer[4] != ' ')) {
                    ok = 0;
                }
                // l points to past last char of the number2
                // convert rx_buffer[i:j) to number
                dec = 1;
                shell_in1 = 0;
                while(j > i) {
                    j -= 1;
                    shell_in1 += (((uint32_t)rx_buffer[j]) - '0') * dec;
                    dec *= 10;
                }
                // convert rx_buffer[k:l) to number
                dec = 1;
                shell_in2 = 0;
                while(l > k) {
                    l -= 1;
                    shell_in2 += (((uint32_t)rx_buffer[l]) - '0') * dec;
                    dec *= 10;
                }
                // test
                //if((shell_in1 == 1) && (shell_in2 == 10)) {
                //    while(1);
                //}
                //
                if(ok) {
                    shell_err = 0;
                    shell_state = CMD4;
                } else {
                    shell_err = 4;
                    shell_state = ERR;
                }
                break;

            case '5':
                shell_state = CMD5;
                break;

            case '6':
                shell_state = CMD6;
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

    // execute command
    switch(shell_state) {
    case ERR:
        shell_state = IDLE;
        __disable_irq();
        tx_idx = 0;
        tx_stop = 0;
        tx_buffer[tx_stop++] = 'E';
        tx_buffer[tx_stop++] = 'R';
        tx_buffer[tx_stop++] = 'R';
        tx_buffer[tx_stop++] = shell_err + '0';
        tx_buffer[tx_stop++] = 0x0D;
        __enable_irq();
        USART2->TDR = 0;
        break;

    case IDLE:
        /* shell_state = IDLE; */
        /* __disable_irq(); */
        /* tx_idx = 0; */
        /* tx_stop = 0; */
        /* tx_buffer[tx_stop++] = 'I'; */
        /* tx_buffer[tx_stop++] = 'D'; */
        /* tx_buffer[tx_stop++] = 'L'; */
        /* tx_buffer[tx_stop++] = 'E'; */
        /* tx_buffer[tx_stop++] = 0x0D; */
        /* __enable_irq(); */
        /* USART2->TDR = 0; */
        break;

    case CMD1:
        shell_state = IDLE;
        fres = f_mount(&fs, "", 1);
        //
        __disable_irq();
        tx_idx = 0;
        tx_stop = snprintf(&tx_buffer[0], ARRSIZE(tx_buffer), "CMD1: f_mount => %d\x0D", fres);
        __enable_irq();
        //
        USART2->TDR = 0;
        break;

    case CMD2:
        shell_state = IDLE;
        fres = f_open(&f, "foo.txt", FA_READ);
        //
        __disable_irq();
        tx_idx = 0;
        tx_stop = snprintf(&tx_buffer[0], ARRSIZE(tx_buffer), "CMD2: f_open => %d\x0D", fres);
        __enable_irq();
        //
        USART2->TDR = 0;
        break;

    case CMD3:
        // TODO: read
        shell_state = IDLE;
        if(f_gets(&line[0], sizeof(line), &f)) {
            line_cnt += 1;
            HAL_Delay(100);
        } else {
            line_cnt = -1;
            snprintf(&line[0], ARRSIZE(line), "<stop>");
            fres = f_close(&f);
        }
        //
        __disable_irq();
        tx_idx = 0;
        tx_stop = snprintf(&tx_buffer[0], ARRSIZE(tx_buffer), "CMD3: f_read => %d, %s\x0D", line_cnt, &line[0]);
        __enable_irq();
        //
        USART2->TDR = 0;
        break;

    case CMD4:
        shell_state = IDLE;
        __disable_irq();
        tx_idx = 0;
        tx_stop = 0;
        tx_buffer[tx_stop++] = 'C';
        tx_buffer[tx_stop++] = 'M';
        tx_buffer[tx_stop++] = 'D';
        tx_buffer[tx_stop++] = '4';
        tx_buffer[tx_stop++] = '=';
        shell_out1 = shell_in1 + shell_in2;
        //
        uint8_t l0 = (shell_out1 / 1) % 10;
        uint8_t l1 = (shell_out1 / 10) % 10;
        uint8_t l2 = (shell_out1 / 100) % 10;
        uint8_t l3 = (shell_out1 / 1000) % 10;
        if(l3) {
            tx_buffer[tx_stop++] = l3 + '0';
            tx_buffer[tx_stop++] = l2 + '0';
            tx_buffer[tx_stop++] = l1 + '0';
            tx_buffer[tx_stop++] = l0 + '0';
        } else if(l2) {
            tx_buffer[tx_stop++] = l2 + '0';
            tx_buffer[tx_stop++] = l1 + '0';
            tx_buffer[tx_stop++] = l0 + '0';
        } else if(l1) {
            tx_buffer[tx_stop++] = l1 + '0';
            tx_buffer[tx_stop++] = l0 + '0';
        } else {
            tx_buffer[tx_stop++] = l0 + '0';
        }
        tx_buffer[tx_stop++] = 0x0D;
        __enable_irq();
        USART2->TDR = 0;
        break;

    case CMD5:
        UART2_Sendchar('R');
        UART2_Sendchar('E');
        UART2_Sendchar('S');
        UART2_Sendchar('E');
        UART2_Sendchar('T');
        UART2_Sendchar('\r');
        HAL_Delay(100);
        NVIC_SystemReset();
        break;

    case CMD6:
        shell_state = IDLE;
        __disable_irq();
        tx_idx = 0;
        tx_stop = 0;
        tx_buffer[tx_stop++] = 'C';
        tx_buffer[tx_stop++] = 'M';
        tx_buffer[tx_stop++] = 'D';
        tx_buffer[tx_stop++] = '6';
        tx_buffer[tx_stop++] = ' ';
        {
            uint32_t u32 = HAL_GetTick();
            uint8_t d3 = ((u32 >> 24) & 0xFF);
            uint8_t d2 = ((u32 >> 16) & 0xFF);
            uint8_t d1 = ((u32 >> 8) & 0xFF);
            uint8_t d0 = ((u32 >> 0) & 0xFF);
            tx_buffer[tx_stop++] = (tbl[(d3 >> 4) & 0x0F]);
            tx_buffer[tx_stop++] = (tbl[(d3 >> 0) & 0x0F]);
            tx_buffer[tx_stop++] = (tbl[(d2 >> 4) & 0x0F]);
            tx_buffer[tx_stop++] = (tbl[(d2 >> 0) & 0x0F]);
            tx_buffer[tx_stop++] = (tbl[(d1 >> 4) & 0x0F]);
            tx_buffer[tx_stop++] = (tbl[(d1 >> 0) & 0x0F]);
            tx_buffer[tx_stop++] = (tbl[(d0 >> 4) & 0x0F]);
            tx_buffer[tx_stop++] = (tbl[(d0 >> 0) & 0x0F]);
        }
        tx_buffer[tx_stop++] = 0x0D;
        __enable_irq();
        USART2->TDR = 0;
        break;

    default:
        shell_state = IDLE;
        break;
    }
}

