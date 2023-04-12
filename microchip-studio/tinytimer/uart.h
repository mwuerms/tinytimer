/**
 * Martin Egli
 * 2023-04-12
 * 
 * uart module
 */

#ifndef _UART_H_
#define _UART_H_

#include <stdint.h>

void uart_Init(void);
void uart_SendCharBlocking(char c);
void uart_SendStringBlocking(char *str);

#endif // _DBG_UART_H_
