#include "Arduino.h"
#include <stdint.h>


#include "./inc/zebrobus.h"
#include "./inc/uart.h"

#include "./inc/zebrobus_vregs.h"

uint8_t vregs[ZEBROBUS_QUEUESIZE];


void ZebroBus::init(Uart *u) {

	uart = u;
	
	// init uart and enable interrupts
	zb_state = ZBSTATE_IDLE;

	vregs[ZEBROBUS_VERSION] = 0x01;
	vregs[ZEBROBUS_CLASS_ID] = ZEBROBUS_CLASS_OBSERVATION;
	vregs[ZEBROBUS_PRODUCT_ID] = ZEBROBUS_DEVICE_OBSERVATIONINTEGRATED1_THIJS;
	vregs[ZEBROBUS_PRODUCT_VERSION] = 0x02;
	vregs[ZEBROBUS_SERIAL_ID] = 0x03;
	vregs[ZEBROBUS_SOFTWARE_VERSION] = 0x02;
	vregs[ZEBROBUS_ADDRESS] = ZEBROBUS_ADDRESS_OBSERVATION;

	vregs[ZEBROBUS_QUICKSTATUS] = 0;
	vregs[ZEBROBUS_SYNC_CTR] = 0;
	vregs[ZEBROBUS_LOOP_CTR] = 0;
	vregs[ZEBROBUS_ERROR_CTR] = 0;
	vregs[ZEBROBUS_LAST_ERROR] = 0;
	
}


void ZebroBus::uart_handler(void) {
	uint8_t data = uart->read();
	
	if (data == ZEBROBUS_OBS_RESET) {
		zb_state = ZBSTATE_IDLE;
		index_autoinc = index;
		return;
	}

	switch (zb_state) {
		case ZBSTATE_IDLE:
			if (data == ZEBROBUS_OBS_MASTER_READ) {
				zb_state = ZBSTATE_MASTER_READ_EXPECT_REG;

			} else if (data == ZEBROBUS_OBS_MASTER_WRITE) {
				zb_state = ZBSTATE_MASTER_WRITE_EXPECT_REG;
			} 
			break;
		case ZBSTATE_MASTER_READ_EXPECT_REG: 
			index = data;
			index_autoinc = data;
			zb_state = ZBSTATE_MASTER_READ_EXPECT_COUNT;
			break;
		case ZBSTATE_MASTER_READ_EXPECT_COUNT:
			num = data;
			zb_state = ZBSTATE_MASTER_READ;
			break;
		case ZBSTATE_MASTER_WRITE_EXPECT_REG:
			index = data;
			index_autoinc = data;
			zb_state = ZBSTATE_MASTER_WRITE_EXPECT_COUNT;
			break;
		case ZBSTATE_MASTER_WRITE_EXPECT_COUNT:
			num = data;
			zb_state = ZBSTATE_MASTER_WRITE;
			break;
		case ZBSTATE_MASTER_WRITE:
			vregs[index_autoinc++] = data; // very unsafe, allows writing to any
										   // byte in register;
			if (index_autoinc == ((index + num)&0xFF)) {
				zb_state = ZBSTATE_IDLE;
			}
			break;
		case ZBSTATE_MASTER_READ:
			break;		// ignore any requests when reading or writing
		default:
			break;

	}
}

uint8_t ZebroBus::read(uint8_t reg) {
	return vregs[reg];
}

void ZebroBus::write(uint8_t reg, uint8_t data) {
	cli();
	vregs[reg] = data;
	sei();
}


void ZebroBus::write_mult(uint8_t reg, uint8_t *data, uint8_t len) {
	uint8_t i = 0;
	cli();
	while (i < len) {
		vregs[reg++] = data[i++];
	}
	sei();
}


void ZebroBus::handle(void) {

	if (zb_state == ZBSTATE_MASTER_READ) {
		while (index_autoinc < index + num) {
			uart->write(vregs[index_autoinc++]);
		}
		zb_state = ZBSTATE_IDLE;
	}
}

