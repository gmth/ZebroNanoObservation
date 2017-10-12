#include "Arduino.h"
#include <Servo.h>

#include "./inc/pinout.h"
#include "./inc/debug.h"
#include "./inc/config.h"
#include "./inc/ledring.h"
#include "./inc/distance.h"
#include "./inc/servomanager.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"

#define BYTE_TO_BINARY(byte)  \
    (byte & 0x80 ? '1' : '0'), \
    (byte & 0x40 ? '1' : '0'), \
    (byte & 0x20 ? '1' : '0'), \
    (byte & 0x10 ? '1' : '0'), \
    (byte & 0x08 ? '1' : '0'), \
    (byte & 0x04 ? '1' : '0'), \
    (byte & 0x02 ? '1' : '0'), \
    (byte & 0x01 ? '1' : '0')

Distance g_dist;
ServoManager g_servomanager;

byte g_distances[DISTANCE_NUM_ANGLES] = {0};
byte g_num_measurement = 0;
byte g_num_measurement_tries = 0;

// temporary value, don't use in interrupt and only use directly after setting
int g_temp;
bool g_use_sweep = false;

void print_distances() {
    int i;
    for (i = 0; i < DISTANCE_NUM_ANGLES; i++) {
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

void handle_serial() {
    if (Serial.available()) {
        switch (Serial.read()) {

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
                break;
        }
    }
}

void setup() {
    int i;
    Serial.begin(38400, SERIAL_8N1);
    Serial.println("Welcome!");
    Serial.println("Setting up");
        
    attachInterrupt(digitalPinToInterrupt(PIN_INT_ECHO), IRQ_on_echo, CHANGE);
    g_servomanager.init();
    Serial.println("Done setting up");
}

void loop() {
    
    g_servomanager.sweep();
    
    g_num_measurement = 0;
    g_num_measurement_tries = 0;
    
    while(g_num_measurement < MEASUREMENT_MAX_NUM && \
            g_num_measurement_tries < MEASUREMENT_MAX_NUM_TRIES) {
    
        if (g_dist.trigger() > 4) {
            clear_echo_pin();
        }

        delay(50);  // enough  for 8.5 meter detection, which is more than the 
                    // sensor can do

        g_temp = g_dist.check_distance(g_num_measurement);

        if (g_temp == 1) {
            g_num_measurement++;
            g_num_measurement_tries++;
        }
        if (g_temp == -1) {
            g_num_measurement_tries++;
        }
    }
    
    g_temp = g_servomanager.get_pos();
    g_distances[g_temp] = g_dist.get_distance_cm(g_num_measurement);
    Serial.print((byte) g_temp);
    Serial.print(": ");
    Serial.print(g_distances[g_temp]);
    Serial.print("\n");

    handle_serial();
    delay(50);
}
