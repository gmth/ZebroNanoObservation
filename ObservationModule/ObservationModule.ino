#include "Arduino.h"
#include <Servo.h>

#include "./inc/pinout.h"
#include "./inc/debug.h"
#include "./inc/config.h"
#include "./inc/ledring.h"
#include "./inc/distance.h"
#include "./inc/servomanager.h"
#include "./inc/error.h"
#include "./inc/uart.h"
#include "./inc/zebrobus.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define UART_DISTANCE_ADDR_OFFSET 16
#define UART_LED_ADDR_OFFSET (32 - SERVO_NUM_ANGLES)

#define BYTE_TO_BINARY(byte)  \
    (byte & 0x80 ? '1' : '0'), \
    (byte & 0x40 ? '1' : '0'), \
    (byte & 0x20 ? '1' : '0'), \
    (byte & 0x10 ? '1' : '0'), \
    (byte & 0x08 ? '1' : '0'), \
    (byte & 0x04 ? '1' : '0'), \
    (byte & 0x02 ? '1' : '0'), \
    (byte & 0x01 ? '1' : '0')

byte g_error = 0;

byte g_distances[SERVO_NUM_ANGLES] = {0};

#define NUM_POSITIONS 5
byte g_used_positions[NUM_POSITIONS] = { 1, 3, 5, 7, 9};
byte g_position_map[SERVO_NUM_ANGLES] = {1, 1, 3, 3, 5, 5, 5, 7, 7, 9, 9};


Distance g_dist;
ServoManager g_servomanager;
LedRing g_ledring = LedRing(g_distances, g_dist.get_max_distance());
ZebroBus g_zebrobus = ZebroBus();
Uart g_uart = Uart();

// temporary value, don't use in interrupt and only use directly after setting
int g_temp;


void Uart_Interrupt(void) {
	g_zebrobus.uart_handler();
}


void setup() {
	g_uart.init();
	g_zebrobus.init(&g_uart);
	g_uart.enable_interrupts(true, false, Uart_Interrupt, NULL);
	g_servomanager.init(g_used_positions, NUM_POSITIONS, &g_zebrobus);
    g_ledring.init();
}

// Handles serial only once per loop, which may cause massive waiting times
// on the other side of the UART. The code looks nice though..
void loop_nice() {
    g_servomanager.sweep();
    g_temp = g_servomanager.get_pos();
    g_distances[g_temp] = g_dist.get_smoothed_distance();
    g_ledring.update_values();
	g_zebrobus.handle();
}

// Handles serial more often and does the smoothing in the main loop.
// Ugly, but works better for communication
void loop_ugly() {
    byte num_measurements = 6;
    byte i;
    byte highest = 0;
    byte lowest = 0;
    int smoothed = 0;
    byte distances[num_measurements] = {0};

/* Handle servo, and do the serial */
    g_servomanager.sweep();
	g_zebrobus.handle();

/* Get multiple measurements, and do serial after each measurement */
    g_temp = g_servomanager.get_pos();
    
    for (i = 0; i < num_measurements; i++) {
        distances[i] = g_dist.get_distance();
        if (distances[i] > distances[highest]) {
            highest = i;
        }
        if (distances[i] < distances[lowest]) {
            lowest = i;
        }
        delay(20);
		g_zebrobus.handle();
    }

    distances[highest] = 0;
    distances[lowest] = 0;

    for (i = 0; i < num_measurements; i++) {
        smoothed += distances[i];
    }
    
    for (i = 0; i < SERVO_NUM_ANGLES; i++) {
        if (g_position_map[i] == g_temp) {
            g_distances[i] = smoothed / (num_measurements - 2);
            //g_distances[i] = distances[highest];
			g_zebrobus.write(VREGS_OBS_DISTANCE0 + i, g_distances[i]);
        }
    }

/* Again handle serial */
	g_zebrobus.handle();

/* Update all LEDS */
    g_ledring.update_values();

}


void loop() {
    loop_ugly();
}
