#ifndef AD7918_INCLUDED
#define AD7918_INCLUDED

#include <stdint.h>

// AD7918 = ADC 10bit, 8channel

#define ADC_IN1     0   // HCU_ACT
#define ADC_IN2     1   // V_MAINS
#define ADC_IN3     2   // I_MAINS
#define ADC_IN4     3   // V_DCPFC
#define ADC_IN5     4   // V_DCOUT
#define ADC_IN6     5   // P15VAC
#define ADC_IN7     6   // HW_VERSION
#define ADC_IN8     7   // TEST_DAC

// module ad7918 public API
void ad7918_init(void);
uint16_t ad7918_read(uint8_t * channel);

#endif // AD7918_INCLUDED

