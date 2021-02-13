#include "ad7918.h"
#include "spibus.h"

// ADR[2:0] = 0 .. 7 => channel address
// PM[1:0] = 11 => normal operation
// PM[1:0] = 10 => full shutdown
// PM[1:0] = 01 => auto shutdown
// PM[1:0] = 00 => invalid
// SEQ:SHADOW = 00 => sequence function not used. The analog input selected for individual conversion.
// SEQ:SHADOW = 01 => selects shadow register for programming. The following write operation (not current) loads shadow register.
// SEQ:SHADOW = 10 => misc mode to update control register without interrupting sequence conversion cycle.
// SEQ:SHADOW = 11 => configuration of ADD2,ADD1,ADD0 channel address to program continuous conversions.
// RANGE = 1 => analog range 0 to Vref, otherwise 0 to 2*Vref
// CODING = 1 => output encoding as binary, otherwise twos complement

#define AD7918_CONFIG(WRITE,SEQ,ADD,PM,SHADOW,RANGE,CODING) ((((WRITE & 1)<<11u) | ((SEQ & 1)<<10u) | ((ADD & 7)<< 6u) | ((PM & 3)<<4u) | ((SHADOW & 1)<<3u) | ((RANGE & 1)<<1) | (CODING & 1)) << 4u)

// AD7918_CONFIG = lambda WRITE,SEQ,ADD,PM,SHADOW,RANGE,CODING: ((((WRITE & 1)<<11) | ((SEQ & 1)<<10) | ((ADD & 7)<< 6) | ((PM & 3)<<4) | ((SHADOW & 1)<<3) | ((RANGE & 1)<<1) | (CODING & 1)) << 4)
// hex(AD7918_CONFIG(1,0,0,3,1,1,1)) => '0x83b0' => 33712
// 0b1000_0011_1011_0000 = 33712

void ad7918_init(void) {
    spibus_send_recv16(0xFFFFu);
    spibus_send_recv16(0xFFFFu);
    spibus_send_recv16(AD7918_CONFIG(1ul,0ul,0ul,3ul,1ul,1ul,1ul));      // write control register ... prepare write to shadow register
    spibus_send_recv16(0xFF00);                                      // write shadow register => set all channels
}

uint16_t ad7918_read(uint8_t * channel) {

    // debug
    /* *channel = (*channel + 1) % 8; */
    /* return (0xAA<<8) | (*channel); */

    uint16_t ret = spibus_send_recv16(0x0000);
    // AD7918
    //ret = [00/A2/A1/A0/D9/D8/D7/D6]/[D5/D4/D3/D2/D1/D0/00/00]
    //ret >>= 2u;     // [00/00/00/A2/A1/A0/D9/D8]/[D7/D6/D5/D4/D3/D2/D1/D0]
    // *channel = (ret >> 10u) & 0x07;
    // ret &= 0x03FF;
    
    // AD7928
    //ret = [00/A2/A1/A0/D11/D10/D9/D8]/[D7/D6/D5/D4/D3/D2/D1/D0]
    ret >>= 0u;     // [00/A2/A1/A0/D11/D10/D9/D8]/[D7/D6/D5/D4/D3/D2/D1/D0]
    *channel = (ret >> 12u) & 0x07;
    ret &= 0x0FFF;

    return ret;
}

