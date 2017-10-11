#include "Arduino.h"
#include "./inc/distance.h"
#include "./inc/pinout.h"

/* brown, green, yellow, red */

Distance::Distance() {
    echo_start = 0;
    echo_end = 0;
    ix_duration = 0;
    new_data_available = false;
    new_distance_available = false;
    triggered = false;
    duration_latest = DISTANCE_DURATION_MAX;
    durations[0] = DISTANCE_DURATION_MAX;
    durations[1] = DISTANCE_DURATION_MAX;
    durations[2] = DISTANCE_DURATION_MAX;
    durations[3] = DISTANCE_DURATION_MAX;
    num_failed_readings = 0;

    pinMode(PIN_INT_ECHO, INPUT);
    pinMode(PIN_DATA_TRIGGER, OUTPUT);
}


int Distance::trigger() {
    // only trigger when it is allowed and we have 
    //! if (triggered) {
    //!     return;
    //! }
    //! triggered = true;
    //! Serial.print("Trig ");
    //! Serial.print(digitalRead(PIN_INT_ECHO));
    //! Serial.print("\n");
    if(digitalRead(PIN_INT_ECHO) == HIGH) {
        num_failed_readings++;
        return num_failed_readings;
    }
    num_failed_readings = 0;
    digitalWrite(PIN_DATA_TRIGGER, LOW);
    delayMicroseconds(2);
    digitalWrite(PIN_DATA_TRIGGER, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_DATA_TRIGGER, LOW);
    return num_failed_readings;
}

int Distance::get_distance_mm() {
    int i;
    unsigned int sum = 0;
    unsigned int num = 0;

    if (!new_distance_available) {
        return -1;
    }

    for (i = 0; i < 4; i++) {
        if (durations[i] > 0) {
            num++;
            sum += (durations[i] * 0.17);
        }
    }

    if (num == 0) {
     // prevent division by zero;
     // shouln't happen because of check !new_distance_avail
        Serial.println("Unexpected division by zero, this is a bug");
        return -2;
    }
    new_distance_available = false;
    return sum/num;
}

int Distance::get_duration() {
    return duration_latest;
}

void Distance::update_buf() {
    if (!new_data_available) {
        return;
    }
    new_data_available = false;
    
    if (duration_latest < DISTANCE_DURATION_MIN) {
        return; // if value impossibly small, discard
    }
    
    if (duration_latest > DISTANCE_DURATION_MAX) {
        duration_latest = DISTANCE_DURATION_MAX;
    }
    
    durations[ix_duration] = duration_latest;
    ix_duration = (ix_duration + 1) % 4;
    new_distance_available = true;
}

void Distance::reset() {
    new_distance_available = false;
    ix_duration = 0;
    durations[0] = 0;
    durations[1] = 0;
    durations[2] = 0;
    durations[3] = 0;
}

void Distance::IRQ_on_echo() {
    switch (digitalRead(PIN_INT_ECHO)) {
        case HIGH:
            echo_start = micros();
        case LOW:
            echo_end = micros();
    }
    duration_latest = echo_end - echo_start;
    new_data_available = true;
    //! triggered = false;
}
