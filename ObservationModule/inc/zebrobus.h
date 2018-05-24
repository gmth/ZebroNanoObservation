#ifndef ZEBROBUS_H
#define ZEBROBUS_H 

#include <stdint.h>
#include "zebrobus_vregs.h"

#include "uart.h"

class ZebroBus {

	private:
		enum ZebrobusState {
			ZBSTATE_IDLE,
			ZBSTATE_MASTER_WRITE_EXPECT_REG,
			ZBSTATE_MASTER_WRITE_EXPECT_COUNT,
			ZBSTATE_MASTER_WRITE,
			ZBSTATE_MASTER_READ_EXPECT_REG,
			ZBSTATE_MASTER_READ_EXPECT_COUNT,
			ZBSTATE_MASTER_READ
		};

		uint8_t vregs[ZEBROBUS_QUEUESIZE];

		volatile enum ZebrobusState zb_state;
		volatile uint8_t index;
		volatile uint8_t index_autoinc;
		uint8_t num;

		Uart *uart;

	public:
		void init(Uart *u);
		uint8_t read(uint8_t reg);
		void write(uint8_t reg, uint8_t data);
		void write_mult(uint8_t reg, uint8_t *data, uint8_t len);
		void handle(void);
		void uart_handler(void);
};



#endif // zebrobus_h
