#include <Servo.h>
#include "Arduino.h"

#define PIN_PWM_SERVO 9

Servo servo;

void servo_set_neutral() {
    servo.write(90);
    delay(100);
}

void setup() {
    servo.attach(PIN_PWM_SERVO);
}

void loop() {
    servo_set_neutral();
    delay(500);
}
