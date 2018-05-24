#ifndef UART_H
#define UART_H 

#include <stdint.h>
#include <stdbool.h>


class Uart {
	public:
		void init();
		void write(char c);
		char read(void);
		void writestr(char *c);
		void enable_interrupts(bool rx, bool tx, void (*rx_handler)(void), void (*tx_handler)(void));
};



#endif /* UART_H */
