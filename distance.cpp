#include "Arduino.h"
#include "./inc/distance.h"
#include "./inc/pinout.h"

/* brown, green, yellow, red */

Distance::Distance() {
    echo_start = 0;
    echo_end = 0;
    new_data_available = false;
    new_distance_available = false;
    triggered = false;
    duration_latest = DISTANCE_DURATION_MAX;
    durations[0] = DISTANCE_DURATION_MAX;
    durations[1] = DISTANCE_DURATION_MAX;
    durations[2] = DISTANCE_DURATION_MAX;
    durations[3] = DISTANCE_DURATION_MAX;
    num_failed_readings = 0;

    duration_latest = 0;

    pinMode(PIN_INT_ECHO, INPUT);
    pinMode(PIN_DATA_TRIGGER, OUTPUT);
}


void Distance::trigger() {
    digitalWrite(PIN_DATA_TRIGGER, LOW);
    delayMicroseconds(2);
    digitalWrite(PIN_DATA_TRIGGER, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_DATA_TRIGGER, LOW);
}

byte Distance::get_distance_cm(byte num_measurements) {
    int i;
    unsigned int sum = 0;
    unsigned int num = 0;

    for (i = 0; i < num_measurements; i++) {
        if (durations[i] > 0) {
            num++;
            sum += (durations[i] * 0.017);
        }
    }
    return (byte) sum/num;
}

int Distance::get_duration() {
    return duration_latest;
}


int Distance::check_distance(byte measurement_ix) {
    int returnval = 0;
    if (!new_data_available) {
        return returnval;
    }
    new_data_available = false;
    
    if (duration_latest < DISTANCE_DURATION_MIN) {
        duration_latest = 0;
        //clear_echo_pin();
        returnval = -1;
        Serial.print(duration_previous);
        Serial.print(" ");
        Serial.println(duration_latest);
        if (duration_previous == 0) {
            duration_estimate = DISTANCE_DURATION_MAX; // 0 means infinity
        }
        else {
            duration_estimate = duration_previous;
        }
    }
    else if (duration_latest > DISTANCE_DURATION_MAX) {
        returnval = 1;
        duration_latest = DISTANCE_DURATION_MAX;
        duration_estimate = DISTANCE_DURATION_MAX;
    }
    else {
        returnval = 1;
        duration_estimate = duration_latest;
    }
    
    durations[measurement_ix] = duration_estimate;
    return returnval;
}

byte Distance::get_max_distance() {
    return (byte) DISTANCE_DURATION_MAX * 0.017;
}

void Distance::IRQ_on_echo() {
    switch (digitalRead(PIN_INT_ECHO)) {
        case HIGH:
            echo_start = micros();
        case LOW:
            echo_end = micros();
    }
    duration_previous = duration_latest;
    duration_latest = echo_end - echo_start;
    new_data_available = true;
}
