/**
 * Martin Egli
 * 2023-04-12
 * 
 * uart module
 */

#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "io.h"
#include "uart.h"

// define UART
#define UART USART0

void uart_Init(void) {
	DBG1_DIR_OUT();
	DBG1_PIN_SET();
	/*DBG1_PIN_CLR();
	DBG1_PIN_SET();
	DBG1_PIN_SET();
	DBG1_PIN_CLR();
	DBG1_PIN_SET();
	DBG1_PIN_CLR();
	DBG1_PIN_SET();
	DBG1_PIN_CLR();
	DBG1_PIN_SET();*/
	UART.CTRLB = USART_TXEN_bm; // enable TXD
	// use alternate output: PA1 for TXD
    PORTMUX.CTRLB = PORTMUX_USART0_ALTERNATE_gc;
	// baudrate: 115200
	// fBaud = (64 * fCLK_PER)/(S * BAUD)
	// S = 16: CTRLB.RXMODE (Normal mode)
	// BAUD = (64 * 2.33 MHz)/(16 * 115200)
	if((FUSE.OSCCFG & FUSE_FREQSEL_gm) == 0x01) {
		// 16MHz (main): fCLK_PER: 2.33MHz/2
		// BAUD = (64 * 2.33 MHz)/(16 * 115200)
		UART.BAUD = 81; // 80.902
	}
	else {
		// 20MHz (main): fCLK_PER: 3.3MHz/2
		// BAUD = (64 * 3.33 MHz)/(16 * 115200)
		UART.BAUD = 116; // 115.625
	}
	// use standard settings
	//UART.CTRLA
	//UART.CTRLC: asynchronous, 8 bit, no parity, 1 stop bit

	// bugfix
	// send any character at the start, so this while wont dead lock
	// while((UART.STATUS & USART_TXCIF_bm) == 0);
	uart_SendChar('0');
}

static inline void uart_TransmitCharBlocking(char c) {
	while((UART.STATUS & USART_TXCIF_bm) == 0);// wait until data can be sent
	UART.STATUS |= USART_TXCIF_bm;	// clear
	UART.TXDATAL = (uint8_t)c;
}

void uart_SendChar(char c) {
	UART.TXDATAL = (uint8_t)c;
}

void uart_SendCharBlocking(char c) {
	uart_TransmitCharBlocking(c);
}

void uart_SendStringBlocking(char *str) {
    while(*str != '\0') {
		uart_TransmitCharBlocking(*str++);
	}
}
