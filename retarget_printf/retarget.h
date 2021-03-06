#ifndef RETARGET_H_
#define RETARGET_H_

#include "stm32f3xx_hal.h"
#include <sys/stat.h>

/*
	char buffer[32] = {0};
	//scanf("%s;\n", &buffer[0]); 	// reads until whitespace
	//gets(&buffer[0]); 			// reads with whitespace but may exceed buffer
	fgets(&buffer[0], 32, stdin);	// reads with whitespace and up to 31 chars
	printf("=> %s\n", &buffer[0]);
*/

#define RETARGET_TO_UART
//#define RETARGET_TO_ITM

#ifdef RETARGET_TO_UART
void RetargetInit_UART(UART_HandleTypeDef *huart);
int _isatty(int fd);
int _write(int fd, char* ptr, int len);
int _close(int fd);
int _lseek(int fd, int ptr, int dir);
int _read(int fd, char* ptr, int len);
int _fstat(int fd, struct stat* st);
#endif 	// RETARGET_TO_UART

#ifdef RETARGET_TO_ITM
void RetargetInit_ITM(void);
int _write(int fd, char* ptr, int len);
#endif 	// RETARGET_TO_ITM

#endif /* RETARGET_H_ */
