#include "../Drivers/CMSIS/Device/ST/STM32L4xx/Include/stm32l4xx.h"
#include "../Inc/main.h"
#include "spibus.h"

void delay(uint32_t x) {
    while(x) {
        x -= 1;
        __asm("nop");
    }
}

void spibus_init_hard_ad7918(void) {
  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  /**SPI1 GPIO Configuration
  PA5   ------> SPI1_SCK
  PA6   ------> SPI1_MISO
  PA7   ------> SPI1_MOSI
  PB0   ------> SPI1_NSS
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_0;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_16BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_HIGH;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_HARD_OUTPUT;
  /* SPI_InitStruct.NSS = LL_SPI_NSS_SOFT; */
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV64;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 7;
  LL_SPI_Init(SPI1, &SPI_InitStruct);
  LL_SPI_SetStandard(SPI1, LL_SPI_PROTOCOL_MOTOROLA);
  LL_SPI_EnableNSSPulseMgt(SPI1);

  LL_SPI_Enable(SPI1);

}


/*
void spibus_init_hard_ad7918(void) {
    // config SPI
    LL_SPI_InitTypeDef SPI_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Peripheral clock enable
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI3);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
    // SPI3 GPIO Configuration
    //PC10   ------> SPI3_SCK
    //PC11   ------> SPI3_MISO
    //PC12   ------> SPI3_MOSI
    GPIO_InitStruct.Pin = SPIBUS_CLK | SPIBUS_MISO | SPIBUS_MOSI;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
    SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
    SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_16BIT;
    SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_HIGH;            // CPOL=1
    SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;                 // CPHA=0
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
    SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV32;           //
    SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;                     // aduc842
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    SPI_InitStruct.CRCPoly = 7;
    LL_SPI_Init(SPI3, &SPI_InitStruct);
    LL_SPI_SetStandard(SPI3, LL_SPI_PROTOCOL_MOTOROLA);     // clock polarity low and high are working
    //LL_SPI_SetStandard(SPI3, LL_SPI_PROTOCOL_TI);           // clock polarity high is not working
    LL_SPI_EnableNSSPulseMgt(SPI3);
    LL_SPI_Enable(SPI3);

    // config chip select lines
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
    //
    LL_GPIO_SetPinMode(GPIOB, SPIBUS_CS1_OUT, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, SPIBUS_CS2_OUT, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOD, SPIBUS_CS3_OUT, LL_GPIO_MODE_OUTPUT);
    //
    LL_GPIO_SetPinOutputType(GPIOB, SPIBUS_CS1_OUT, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinOutputType(GPIOB, SPIBUS_CS2_OUT, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinOutputType(GPIOD, SPIBUS_CS3_OUT, LL_GPIO_OUTPUT_PUSHPULL);
    //
    LL_GPIO_SetOutputPin(GPIOB, SPIBUS_CS1_OUT);
    LL_GPIO_SetOutputPin(GPIOB, SPIBUS_CS2_OUT);
    LL_GPIO_SetOutputPin(GPIOD, SPIBUS_CS3_OUT);

}
*/

void spibus_init_soft(void) {

    // config chip select lines
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

    //
    LL_GPIO_SetPinMode(GPIOA, SPIBUS_CLK, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOA, SPIBUS_MOSI, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOA, SPIBUS_MISO, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinMode(GPIOB, SPIBUS_CS_OUT, LL_GPIO_MODE_OUTPUT);
    //
    LL_GPIO_SetPinOutputType(GPIOA, SPIBUS_CLK, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinOutputType(GPIOA, SPIBUS_MOSI, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinOutputType(GPIOA, SPIBUS_MISO, LL_GPIO_PULL_NO);
    /* LL_GPIO_SetPinOutputType(GPIOA, SPIBUS_MISO, LL_GPIO_PULL_UP); */
    LL_GPIO_SetPinOutputType(GPIOB, SPIBUS_CS_OUT, LL_GPIO_OUTPUT_PUSHPULL);
    //
    LL_GPIO_SetPinSpeed(GPIOA, SPIBUS_CLK, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinSpeed(GPIOA, SPIBUS_MOSI, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinSpeed(GPIOA, SPIBUS_MISO, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinSpeed(GPIOB, SPIBUS_CS_OUT, LL_GPIO_SPEED_FREQ_HIGH);
    //
    LL_GPIO_SetOutputPin(GPIOA, SPIBUS_CLK);
    LL_GPIO_ResetOutputPin(GPIOA, SPIBUS_MOSI);
    LL_GPIO_SetOutputPin(GPIOB, SPIBUS_CS_OUT);
}

// used by all devices
uint16_t spibus_send_recv16(uint16_t dataOut) {
    uint16_t dataIn = 0x0000;

    // disable chip select
    /* LL_GPIO_SetOutputPin(GPIOB, SPIBUS_CS_OUT); */

    /* dataIn = spibus_send_recv16_ad7918_soft(dataOut); */
    dataIn = spibus_send_recv16_ad7918_hard(dataOut);

    // disable chip select
    /* LL_GPIO_SetOutputPin(GPIOB, SPIBUS_CS_OUT); */

    return dataIn;
}

uint16_t spibus_send_recv16_ad7918_hard(uint16_t data) {

    // chip select => enable ad7918
    LL_GPIO_ResetOutputPin(GPIOB, SPIBUS_CS_OUT);

    while(!LL_SPI_IsActiveFlag_TXE(SPI1));
    LL_SPI_TransmitData16(SPI1, data);
    /* SPI1->DR = data; */

    while(!LL_SPI_IsActiveFlag_RXNE(SPI1));
    data = LL_SPI_ReceiveData16(SPI1);
    /* data = SPI1->DR; */

    while(LL_SPI_IsActiveFlag_BSY(SPI1));

    // chip select => disable ad7918
    LL_GPIO_SetOutputPin(GPIOB, SPIBUS_CS_OUT);

    return data;
}

uint16_t spibus_send_recv16_ad7918_soft(uint16_t dataOut) {

    uint16_t dataIn = 0;

    // chip select => enable ad7918
    LL_GPIO_ResetOutputPin(GPIOB, SPIBUS_CS_OUT);

    /* __asm("nop"); */
    /* __asm("nop"); */
    /* __asm("nop"); */
    __asm("nop");
    __asm("nop");
    __asm("nop");

    for(uint8_t i=0; i<16; i+=1) {

        // set data out
        if(dataOut & 0x8000u) {
            LL_GPIO_SetOutputPin(GPIOA, SPIBUS_MOSI);
        } else {
            LL_GPIO_ResetOutputPin(GPIOA, SPIBUS_MOSI);
        }
        dataOut <<= 1;  // shift after eval, last iteration is nop

        // clock high
        /* __asm("nop"); */
        /* __asm("nop"); */
        /* __asm("nop"); */
        /* __asm("nop"); */
        /* __asm("nop"); */
        /* __asm("nop"); */
        /* __asm("nop"); */
        __asm("nop");
        __asm("nop");
        __asm("nop");

        // set falling clock edge
        LL_GPIO_ResetOutputPin(GPIOA, SPIBUS_CLK);

        // sample data
        dataIn <<= 1;   // shift before eval, first iteration is nop
        if(LL_GPIO_IsInputPinSet(GPIOA, SPIBUS_MISO)) {
            dataIn |= 0x01u;
        }

        // clock low
        /* __asm("nop"); */
        /* __asm("nop"); */
        /* __asm("nop"); */
        /* __asm("nop"); */
        /* __asm("nop"); */
        /* __asm("nop"); */
        /* __asm("nop"); */
        __asm("nop");
        __asm("nop");
        __asm("nop");

        // set rising clock edge
        LL_GPIO_SetOutputPin(GPIOA, SPIBUS_CLK);
    }

    // chip select => disable ad7918
    LL_GPIO_SetOutputPin(GPIOB, SPIBUS_CS_OUT);

    return dataIn;
}

