#include "Arduino.h"
#include <Servo.h>

#include "./inc/pinout.h"
#include "./inc/debug.h"
#include "./inc/config.h"
#include "./inc/ledring.h"
#include "./inc/distance.h"
#include "./inc/servomanager.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define UART_ADDR_OFFSET 15;

#define BYTE_TO_BINARY(byte)  \
    (byte & 0x80 ? '1' : '0'), \
    (byte & 0x40 ? '1' : '0'), \
    (byte & 0x20 ? '1' : '0'), \
    (byte & 0x10 ? '1' : '0'), \
    (byte & 0x08 ? '1' : '0'), \
    (byte & 0x04 ? '1' : '0'), \
    (byte & 0x02 ? '1' : '0'), \
    (byte & 0x01 ? '1' : '0')

byte g_distances[SERVO_NUM_ANGLES] = {0};
byte g_num_measurement = 0;

Distance g_dist;
ServoManager g_servomanager;
LedRing g_ledring = LedRing(g_distances, g_dist.get_max_distance());

// temporary value, don't use in interrupt and only use directly after setting
int g_temp;
bool g_use_sweep = false;

void print_distances() {
    int i;
    for (i = 0; i < SERVO_NUM_ANGLES; i++) {
        Serial.print(g_distances[i]);
        Serial.print(" ");
    }
    Serial.print("\n");
}

void IRQ_on_echo() {
    g_dist.IRQ_on_echo();
}

// There is no timeout on the ultrasound sensor; when it receives no echo,
// the echo pin stays high. This disables the interrupt, forces the pin low,
// and enables the interrupt again.
void clear_echo_pin() {
    detachInterrupt(digitalPinToInterrupt(PIN_INT_ECHO));
    delay(30);
    pinMode(PIN_INT_ECHO, OUTPUT);
    digitalWrite(PIN_INT_ECHO, LOW);
    delay(30);
    pinMode(PIN_INT_ECHO, INPUT);
    attachInterrupt(digitalPinToInterrupt(PIN_INT_ECHO), IRQ_on_echo, CHANGE);
}

bool handle_pc_commands(byte b) {
    switch (b) {
        case 'a':
            g_servomanager.step_left();
            Serial.println(g_servomanager.get_pos());
            break;
        case 'd':
            g_servomanager.step_right();
            Serial.println(g_servomanager.get_pos());
            break;
        case 'n':
            g_servomanager.set_neutral();
            break;
        case 'p':
            print_distances();
            break;
        case 's':
            g_servomanager.toggle_sweep();
            break;
        default:
            return false;
            break;
    }
    return true;
}

bool handle_pi_commands(byte b) {

    switch(b) {
        case 'r':
            byte addr;
            byte addr_with_offset;
            // blocking wait for an address to be sent:
            while ((addr_with_offset = Serial.read()) == -1);
            addr = addr_with_offset - UART_ADDR_OFFSET;
            
            // wait for last zero byte, and carriage returns
            // TODO: FIX
            while (Serial.read() != 0);
            while (Serial.read() != '\r');
            while (Serial.read() != '\n');

            if (addr < SERVO_NUM_ANGLES) {   // we're ok
                Serial.write('s');              // success
                Serial.write(addr_with_offset);
                Serial.write(g_distances[addr]);
                Serial.write('\r');
                Serial.write('\n');
            }
            break;

        case 'w':
            break;
        
        default:
            return false;
    }
    return true;
}

void handle_serial() {
    bool ret = true;
    char b;
    if (Serial.available() > 2) {
        b = Serial.read();
        ret = handle_pi_commands(b);
    }
    
    if (!ret) {
        handle_pc_commands(b);
    }

    else if (Serial.available()) {
        handle_pc_commands(Serial.read());
    }
}

void setup() {
    int i;
    Serial.begin(38400, SERIAL_8N1);
    Serial.println("Welcome!");
    Serial.println("Setting up");
        
    attachInterrupt(digitalPinToInterrupt(PIN_INT_ECHO), IRQ_on_echo, CHANGE);
    g_servomanager.init();
    g_ledring.init();
    Serial.println("Done setting up");
}

void loop() {
    
    g_servomanager.sweep();
    
    g_num_measurement = 0;
    
    while(g_num_measurement < MEASUREMENT_MAX_NUM) {
    
        g_dist.trigger();

        delay(25);  // enough  for 4.5 meter detection, which is more than the 
                    // sensor can do

        g_temp = g_dist.check_distance(g_num_measurement);

        if (g_temp == 1) {
            g_num_measurement++;
        }
        if (g_temp == -1) {
            clear_echo_pin();
            g_num_measurement++;
        }
    }
    
    g_temp = g_servomanager.get_pos();
    g_distances[g_temp] = g_dist.get_distance_cm(g_num_measurement);
    Serial.print((byte) g_temp);
    Serial.print(": ");
    Serial.print(g_distances[g_temp]);
    Serial.print("\n");

    handle_serial();
    g_ledring.update_values();
    delay(50);
}
