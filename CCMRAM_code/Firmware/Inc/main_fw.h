#ifndef INC_MAIN_FW_H_
#define INC_MAIN_FW_H_

void main_fw(void);

extern volatile uint32_t rcc_interrupt_counter;
extern volatile uint32_t tim7_interrupt_counter;

#endif /* INC_MAIN_FW_H_ */
