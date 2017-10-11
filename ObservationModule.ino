#include "Arduino.h"
#include <Servo.h>

#include "./inc/pinout.h"
#include "./inc/debug.h"
#include "./inc/config.h"
#include "./inc/ledring.h"
#include "./inc/distance.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define DISTANCE_NUM_ANGLES 11

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

/* Left to right */
unsigned int g_positions[] = {165, 150, 135, 120, 105, 90, 75, 60, 45, 30, 15};
unsigned int g_distances[DISTANCE_NUM_ANGLES] = {0};
int g_pos_ix = DISTANCE_NUM_ANGLES / 2;
int g_pos_ix_previous = g_pos_ix;

int g_num_measurement = 0;
int g_num_measurement_tries = 0;

// temporary value, don't use in interrupt and only use directly after setting
int g_temp;

Servo g_servo;            // Must be a global variable, else the servo
                          // forces itself all the way to the right, it seems..
int g_servo_pos = g_positions[DISTANCE_NUM_ANGLES/2];

void servo_step_left() {
    if (g_pos_ix > 0) {
        g_pos_ix--;
    }
    g_servo.write(g_positions[g_pos_ix]);
    delay(50);
}

void servo_step_right() {
    if (g_pos_ix < 11) {
        g_pos_ix++;
    }
    g_servo.write(g_positions[g_pos_ix]);
    delay(50);
}

void servo_set_neutral() {
    g_pos_ix = DISTANCE_NUM_ANGLES / 2;
    g_servo.write(g_positions[g_pos_ix]);
    delay(100);
}

void print_distances() {
    int i;
    for (i = 0; i < DISTANCE_NUM_ANGLES; i++) {
        Serial.print(g_distances[g_pos_ix]);
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

void sweep() {
    if (g_pos_ix < g_pos_ix_previous) {
        g_pos_ix_previous = g_pos_ix;
        g_pos_ix--;
        if (g_pos_ix < 0) {
            g_pos_ix = 1;
        }
    }

    else {
        g_pos_ix_previous = g_pos_ix;
        g_pos_ix++;
        if (g_pos_ix == DISTANCE_NUM_ANGLES) {
            g_pos_ix = DISTANCE_NUM_ANGLES - 2;
        }
    }
}

void handle_serial() {
    if (Serial.available()) {
        switch (Serial.read()) {

            case 'a':
                servo_step_left();
                Serial.println(g_servo_pos);
                break;
            case 'd':
                servo_step_right();
                Serial.println(g_servo_pos);
                break;
            case 'n':
                servo_set_neutral();
                break;
            case 'p':
                print_distances();
                break;
            default:
                break;
        }
    }
}

void setup() {
    int i;
    Serial.begin(38400);
    Serial.println("Welcome!");
    Serial.println("Setting up");
        
    attachInterrupt(digitalPinToInterrupt(PIN_INT_ECHO), IRQ_on_echo, CHANGE);
    g_servo.attach(PIN_PWM_SERVO);

    Serial.println("Done setting up");
}

void loop() {

    sweep();
    g_servo.write(g_positions[g_pos_ix]);
    
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

    g_distances[g_pos_ix] = g_dist.get_distance_mm(g_num_measurement);
    Serial.print(g_pos_ix);
    Serial.print(": ");
    Serial.print(g_distances[g_pos_ix]);
    Serial.print("\n");

    handle_serial();
    delay(50);
}
