#include "Arduino.h"
#include <Servo.h>

#include "./inc/pinout.h"
#include "./inc/debug.h"
#include "./inc/config.h"
#include "./inc/ledring.h"
#include "./inc/distance.h"
#include "./inc/servomanager.h"
#include "./inc/error.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define UART_DISTANCE_ADDR_OFFSET 15
#define UART_LED_ADDR_OFFSET 15

#define BYTE_TO_BINARY(byte)  \
    (byte & 0x80 ? '1' : '0'), \
    (byte & 0x40 ? '1' : '0'), \
    (byte & 0x20 ? '1' : '0'), \
    (byte & 0x10 ? '1' : '0'), \
    (byte & 0x08 ? '1' : '0'), \
    (byte & 0x04 ? '1' : '0'), \
    (byte & 0x02 ? '1' : '0'), \
    (byte & 0x01 ? '1' : '0')

struct Packet {
    byte header;
    byte addr;
    byte data;
    byte cr;
    byte lf;
};
byte g_error = 0;
byte g_distances[SERVO_NUM_ANGLES] = {0};
struct Packet packet_receive;
struct Packet packet_send;

Distance g_dist;
ServoManager g_servomanager;
LedRing g_ledring = LedRing(g_distances, g_dist.get_max_distance());

// temporary value, don't use in interrupt and only use directly after setting
int g_temp;

void serial_send_packet() {
    packet_send.cr = '\r';
    packet_send.lf = '\n';
    Serial.write(packet_send.header);
    Serial.write(packet_send.addr);
    Serial.write(packet_send.data);
    Serial.write(packet_send.cr);
    Serial.write(packet_send.lf);
}

void serial_handle_read() {
    bool valid = true;
    packet_receive.addr = Serial.read();
    packet_receive.data = Serial.read();
    packet_receive.cr = Serial.read();
    packet_receive.lf = Serial.read();

    if (packet_receive.addr - UART_DISTANCE_ADDR_OFFSET >= SERVO_NUM_ANGLES) {
        g_error |= ERROR_UART_INVALID_ADDR;
        packet_send.header = 'f';
        valid = false;
    }
    if (packet_receive.data != 0) {
        g_error |= ERROR_UART_INVALID_DATA;
        packet_send.header = 'f';
        valid = false;
    }
    if (packet_receive.cr != '\r') {
        g_error |= ERROR_UART_INVALID_CR;
        packet_send.header = 'f';
        valid = false;
    }
    if (packet_receive.lf != '\n') {
        g_error |= ERROR_UART_INVALID_LF;
        packet_send.header = 'f';
        valid = false;
    }
    if (valid) {
        packet_send.header = 's';
        packet_send.data = g_distances[packet_receive.addr - UART_DISTANCE_ADDR_OFFSET];
        g_error &= ~(ERROR_UART_INVALID_ADDR);
        g_error &= ~(ERROR_UART_INVALID_DATA);
        g_error &= ~(ERROR_UART_INVALID_CR);
        g_error &= ~(ERROR_UART_INVALID_LF);
    }
    
    packet_send.addr = packet_receive.addr;
    serial_send_packet();
}

void serial_handle_write() {
    bool valid = true;
    packet_receive.addr = Serial.read();
    packet_receive.data = Serial.read();
    packet_receive.cr = Serial.read();
    packet_receive.lf = Serial.read();
    
    if (packet_receive.addr - UART_LED_ADDR_OFFSET >= NUM_LEDS) {
        g_error |= ERROR_UART_INVALID_ADDR;
        packet_send.header = 'f';
        valid = false;
    }
    if (packet_receive.cr != '\r') {
        g_error |= ERROR_UART_INVALID_CR;
        packet_send.header = 'f';
        valid = false;
    }
    if (packet_receive.lf != '\n') {
        g_error |= ERROR_UART_INVALID_LF;
        packet_send.header = 'f';
        valid = false;
    }
    if (valid) {
        packet_send.header = 's';
        g_ledring.set_led(packet_receive.data, packet_receive.addr - UART_LED_ADDR_OFFSET);
        g_error &= ~(ERROR_UART_INVALID_ADDR);
        g_error &= ~(ERROR_UART_INVALID_DATA);
        g_error &= ~(ERROR_UART_INVALID_CR);
        g_error &= ~(ERROR_UART_INVALID_LF);
    }
    
    packet_send.data = packet_receive.data;
    packet_send.addr = packet_receive.addr;
    serial_send_packet();
}

void serial_handle_neutral() {
    bool valid = true;
    packet_receive.addr = Serial.read();
    packet_receive.data = Serial.read();
    packet_receive.cr = Serial.read();
    packet_receive.lf = Serial.read();
    
    if (packet_receive.addr != 0) {
        g_error |= ERROR_UART_INVALID_ADDR;
        packet_send.header = 'f';
        valid = false;
    }
    if (packet_receive.data != 0) {
        g_error |= ERROR_UART_INVALID_DATA;
        packet_send.header = 'f';
        valid = false;
    }
    if (packet_receive.cr != '\r') {
        g_error |= ERROR_UART_INVALID_CR;
        packet_send.header = 'f';
        valid = false;
    }
    if (packet_receive.lf != '\n') {
        g_error |= ERROR_UART_INVALID_LF;
        packet_send.header = 'f';
        valid = false;
    }
    if (valid) {
        packet_send.header = 's';
        g_servomanager.set_neutral();
        g_error &= ~(ERROR_UART_INVALID_ADDR);
        g_error &= ~(ERROR_UART_INVALID_DATA);
        g_error &= ~(ERROR_UART_INVALID_CR);
        g_error &= ~(ERROR_UART_INVALID_LF);
    }
    packet_send.data = packet_receive.data;
    packet_send.addr = packet_receive.addr;
    serial_send_packet();
}

void serial_handle_sweep() {
    bool valid = true;
    packet_receive.addr = Serial.read();
    packet_receive.data = Serial.read();
    packet_receive.cr = Serial.read();
    packet_receive.lf = Serial.read();
    
    if (packet_receive.addr != 0) {
        g_error |= ERROR_UART_INVALID_ADDR;
        packet_send.header = 'f';
        valid = false;
    }
    if (packet_receive.data != 0) {
        g_error |= ERROR_UART_INVALID_DATA;
        packet_send.header = 'f';
        valid = false;
    }
    if (packet_receive.cr != '\r') {
        g_error |= ERROR_UART_INVALID_CR;
        packet_send.header = 'f';
        valid = false;
    }
    if (packet_receive.lf != '\n') {
        g_error |= ERROR_UART_INVALID_LF;
        packet_send.header = 'f';
        valid = false;
    }
    if (valid) {
        packet_send.header = 's';
        g_servomanager.toggle_sweep();
        g_error &= ~(ERROR_UART_INVALID_ADDR);
        g_error &= ~(ERROR_UART_INVALID_DATA);
        g_error &= ~(ERROR_UART_INVALID_CR);
        g_error &= ~(ERROR_UART_INVALID_LF);
    }
    packet_send.data = packet_receive.data;
    packet_send.addr = packet_receive.addr;
    serial_send_packet();
}

void serial_handle_query() {
    // Dont check the data and addr stuff etc, because 
    // that can mess up the error message (maybe there
    // was no error, but because of this message there is)
    // Its a tradeoff that might have to be revised;
    packet_receive.addr = Serial.read();
    packet_receive.data = Serial.read();
    packet_receive.cr = Serial.read();
    packet_receive.lf = Serial.read();
    packet_send.header = 's';
    packet_send.addr = 0;
    packet_send.data = g_error;
    serial_send_packet();
}


void serial_handle() {

    // for UART, use the serial library. print sends nothing extra,
    // println adds a linefeed, and a carriage return
    
    if (Serial.available() > 4) {
        switch(Serial.read()) {
            case 'r':
                serial_handle_read();
                break;
            case 'w':
                serial_handle_write();
                break;
            case 'n':
                serial_handle_neutral();
                g_servomanager.set_neutral();
                break;
            case 's':
                serial_handle_sweep();
                g_servomanager.toggle_sweep();
                break;
            case 'q':
                serial_handle_query();
                break;
            default:
                break;
        }
    }
}

void setup() {
    int i;
    Serial.begin(38400, SERIAL_8N1);
    g_servomanager.init();
    g_ledring.init();
}

void loop() {
    g_servomanager.sweep();
    g_temp = g_servomanager.get_pos();
    g_distances[g_temp] = g_dist.get_distance();
    serial_handle();
    g_ledring.update_values();
    delay(50);
}
