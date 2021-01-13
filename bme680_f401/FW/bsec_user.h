#ifndef BSEC_USER_INCLUDED
#define BSEC_USER_INCLUDED

// 521177 - 431331 - good?
// 297625 - 213212 - average ?
// 148977 - 108042 - little bad ?
// 75010 - 54586 - bad ?
// 37395 - 27080 - worse ?
// 18761 - 13591 - very bad ?
// 9008 - 8371 - can’t see the exit ?

#define IAQ_THRES_0     521177
#define IAQ_THRES_1     431331
#define IAQ_THRES_2     297625
#define IAQ_THRES_3     213212
#define IAQ_THRES_4     148977
#define IAQ_THRES_5     108042
#define IAQ_THRES_6     75010
#define IAQ_THRES_7     54586
#define IAQ_THRES_8     37395
#define IAQ_THRES_9     27080
#define IAQ_THRES_10    18761
#define IAQ_THRES_11    13591
#define IAQ_THRES_12    9008
#define IAQ_THRES_13    8371

const char * iaq_text(uint32_t res);

int8_t user_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
int8_t user_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
void user_delay_ms(uint32_t period);

#endif  // BSEC_USER_INCLUDED

