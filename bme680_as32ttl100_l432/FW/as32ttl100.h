#ifndef AS32TTL100_INCLUDED
#define AS32TTL100_INCLUDED

#include <stdint.h>

// TODO: currently the AS32-TTL100 transmitter and receiver modules are configured on PC with following settings
// both modules address 00 01
// channel 410+0x17 = 433MHz
// 2.4kBaud UART, 8 data bits, no parity, 1 stop bit
// 9.6kBaud air speed
// transparent transmission => data send == data received
// TXD,AUX = push pull, RXD = pullup
// wake up 250ms
// tranmitting power 11dBm
// C1 C1 C1 => C0 00 01 0A 17 43

// operation mode = inputs [M1:M0] with weak pullup
// AUX output should be checked before switching operation mode
// when AUX is low => module is busy
#define MODE_GENERAL    0
#define MODE_POWSAVE    1
#define MODE_WAKEUP     2
#define MODE_SLEEP      3

// command
#define CMD_SET_SAVE_PARAM      0xC0    // 0xC0, ADDH, ADDL, SPEED, OPTION => OK, ERROR
#define CMD_SET_NOSAVE_PARAM    0xC2    // 0xC2, ADDH, ADDL, SPEED, OPTION => OK, ERROR
#define CMD_CONFIG_ENCRYPTION   0xC6    // 0xC6, 16 bytes of encryption key => OK
#define CMD_READ_PARAMS         0xC1    // 0xC1, 0xC1, 0xC1 => 6 bytes of current parameters
#define CMD_READ_HW_VERSION     0xC3    // 0xC3, 0xC3, 0xC3 => "AS32-TTL-100-V3.0"
#define CMD_READ_SW_VERSION     0xF3    // 0xF3, 0xF3, 0xF3 => "software version"
#define CMD_RESET               0xC4    // 0xC4, 0xC4, 0xC4 => OK
#define CMD_READ_SUPVOLTAGE     0xC5    // 0xC5, 0xC5, 0xC5 => C5,VH,VL ... e.g. C5,0C,1C => 0x0C1C = 3100 = 3.1V
#define CMD_RESTORE_DEFAULTS    0xC9    // 0xC9, 0xC9, 0xC9 => OK
#define CMD_HANDSHAKE           0xE1    // 0xE1, 0xE1, 0xE1 => OK ... change baudrate and send this command until OK received

// READ RSSI of the current data signal directly = 0xAF,0xAF,0x73,0x00,0xAF,0xF3
// READ RSSI of the environmental signal directly = 0xAF,0xAF,0x74,0x00,0xAF,0xF4

struct {
    uint8_t mode;
} as32ttl100_t;

#endif // AS32TTL100_INCLUDED

