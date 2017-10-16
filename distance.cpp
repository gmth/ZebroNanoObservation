#include "Arduino.h"
#include "./inc/distance.h"
#include "./inc/pinout.h"

/* brown, green, yellow, red */

Distance::Distance() {
    duration = DISTANCE_DURATION_MAX;
    pinMode(PIN_INT_ECHO, INPUT);
    pinMode(PIN_DATA_TRIGGER, OUTPUT);
}
// There is no timeout on the ultrasound sensor; when it receives no echo,
// the echo pin stays high. This disables the interrupt, forces the pin low,
// and enables the interrupt again.
void Distance::clear_echo_pin() {
    pinMode(PIN_INT_ECHO, OUTPUT);
    delay(10);
    digitalWrite(PIN_INT_ECHO, LOW);
    delay(10);
    pinMode(PIN_INT_ECHO, INPUT);
}

byte Distance::get_max_distance() {
    return (byte) DISTANCE_DURATION_MAX * 0.017;
}

byte Distance::get_smoothed_distance() {
    int smoothed = 0;
    int i;

    for(i = 0; i < 4; i++) {
        smoothed += get_distance();
    }
    return smoothed/4;
}

byte Distance::get_distance() {
    unsigned int distance;
    // Clears the trigPin
    digitalWrite(PIN_DATA_TRIGGER, LOW);
    delayMicroseconds(2);

    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(PIN_DATA_TRIGGER, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_DATA_TRIGGER, LOW);

    duration_prev = duration;

    duration = pulseIn(PIN_INT_ECHO, HIGH);

    if (duration == 0) {
        clear_echo_pin();

        if (duration_prev== 0) {
            duration_act = duration;
        }
        else {
            duration_act = duration_prev;
        }
    }
    else {
        duration_act = duration;
    }
    
    if (duration_act < DISTANCE_DURATION_MIN) {
        duration_act = DISTANCE_DURATION_MAX; // low duration is inf
    }
    else if (duration_act > DISTANCE_DURATION_MAX) {
        duration_act = DISTANCE_DURATION_MAX;
    }
    distance = duration_act * 0.017;
    
    return (byte) distance;
}
