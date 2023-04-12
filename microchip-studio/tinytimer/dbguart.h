/**
 * Martin Egli
 * 2023-04-12
 * 
 * debug uart module
 */

#ifndef _DBG_UART_H_
#define _DBG_UART_H_

#include "project.h"

#if DBG_UART()

#include "uart.h"
#define dbguart_Init() uart_Init()
#define dbguart_SendChar(c) uart_SendCharBlocking(c)
#define dbguart_SendString(str) uart_SendStringBlocking(str)

#else

#define dbguart_Init()
#define dbguart_SendChar(...)
#define dbguart_SendString(...)

#endif

#endif // _DBG_UART_H_
