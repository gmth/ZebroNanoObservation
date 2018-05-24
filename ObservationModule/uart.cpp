/**
 * @file uart.c
 * @author gmth
 * @brief Functions to write bytes of UART
 * @attention This module uses the UART0 peripheral
 */
#include <stdlib.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "inc/uart.h"

#define FOSC 16000000 // Clock Speed
#define BAUD 38400
#define MYUBRR (((FOSC/16)/BAUD)-1)

void (*IRQ_RX_Handler)(void) = NULL;
void (*IRQ_TX_Handler)(void) = NULL;

ISR (USART_RX_vect) {
	if (IRQ_RX_Handler != NULL) {
		IRQ_RX_Handler();
	}
}

ISR (USART_TX_vect) {
	if (IRQ_TX_Handler != NULL) {
		IRQ_TX_Handler();
	}
}

/**
 * @brief Initialise the UART0 peripheral
 *
 * Initialise the UART0 peripheral to 8n1 at the defined BAUD rate.
 */
void Uart::init() {
	
	// set baud rate
	UBRR0H = MYUBRR >> 8;
	UBRR0L = MYUBRR & 0xFF;
	
	// enable rx and tx, no interrupts
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);

	// async, no parity, 1 stop bit, 8 bit char, 0 clock in async
	UCSR0C = 0b00000110;

}


void Uart::enable_interrupts(bool rx, bool tx, void (*rx_handler)(void), void (*tx_handler)(void)) {
	cli();
	if (rx) {
		UCSR0B |= (1<<RXCIE0);
		IRQ_RX_Handler = rx_handler;
	}
	if (tx) {
		UCSR0B |= (1<<TXCIE0);
		IRQ_TX_Handler = tx_handler;
	}
	sei();
}


/**
 * @brief Write a single byte over UART
 * @param c The byte to write
 */
void Uart::write(char c) {

	while (!( UCSR0A & (1 << UDRE0)));

	// set error bit to zero
	UCSR0A &= ~(1 << FE0);
	
	UDR0 = c;
}

/**
 * @brief Write a single byte over UART
 * @param c The byte to write
 */
char Uart::read(void) {
	return UDR0;
}

/**
 * @brief Write a null-terminated array of characters over UART
 * @param *c The null-terminated array to write
 */
void Uart::writestr(char *c) {
	while (*(c) != '\0') {
		write(*(c++));
	}
}
