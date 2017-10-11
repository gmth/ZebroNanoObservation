#include "Arduino.h"
#include <Servo.h>

#include "./inc/pinout.h"
#include "./inc/debug.h"
#include "./inc/config.h"
#include "./inc/ledring.h"
#include "./inc/distance.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define DISTANCE_NUM_ANGLES 7
#define SERVO_ANGLE_NEUTRAL 128
#define SERVO_ANGLE_MIN 88
#define SERVO_ANGLE_MAX 168
#define SERVO_ANGLE_STEP ((SERVO_ANGLE_NEUTRAL - SERVO_ANGLE_MIN)/((DISTANCE_NUM_ANGLES-1) / 2))

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
/* positions: -75, -60, -45, -30, -15, 0, 15, 30, 45, 60, 75 */
/* indices:     0    1     2   3    4  5  6   7   8   9   10 */

unsigned int g_positions[DISTANCE_NUM_ANGLES] = {0};
unsigned int g_distances[DISTANCE_NUM_ANGLES] = {0};
int g_distance_ix = 0;

Servo g_servo;            // Must be a global variable, else the servo
                          // forces itself all the way to the right, it seems..
int g_servo_pos = SERVO_ANGLE_NEUTRAL;

void servo_step_left() {
    //! if (g_servo_pos < SERVO_ANGLE_MAX) {
    //!     g_servo_pos+=SERVO_ANGLE_STEP;
    //!     g_distance_ix++;
    //!     g_dist.reset();
    //! }

    g_servo_pos++;
    g_servo.write(g_servo_pos);
    delay(50);
}

void servo_step_right() {
    // if (g_servo_pos > SERVO_ANGLE_MIN) {
    //     g_servo_pos-=SERVO_ANGLE_STEP;
    //     g_distance_ix--;
    //     g_dist.reset();
    // }
    g_servo_pos--;
    g_servo.write(g_servo_pos);
    delay(50);
}

void servo_set_neutral() {
    g_servo_pos = SERVO_ANGLE_NEUTRAL;
    g_distance_ix = DISTANCE_NUM_ANGLES / 2;
    g_servo.write(g_servo_pos);
    g_dist.reset();
    delay(100);
}

void print_distances() {
    int i;
    for (i = 0; i < DISTANCE_NUM_ANGLES; i++) {
        Serial.print(g_distances[g_distance_ix]);
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
    if (g_dist.new_distance_available) {
        g_distances[g_distance_ix] = g_dist.get_distance_mm();
        Serial.print("Distance: ");
        Serial.print(g_distances[g_distance_ix]);
        Serial.print(" ");
        Serial.print(g_distances[g_distance_ix] > 0);
        Serial.print("\n");
    }
    if (g_dist.trigger() > 4) {
        clear_echo_pin();
    }

    g_dist.update_buf();

    if (Serial.available()) {
        switch (Serial.read()) {

            case 'a':
                servo_step_left();
                break;
            case 'd':
                servo_step_right();
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
    delay(50);
}
