#include "inc/servomanager.h"
#include "inc/pinout.h"
#include "Arduino.h"

void ServoManager::init() {
    pos_ix = SERVO_NUM_ANGLES / 2;
    pos_ix_previous = pos_ix;
    servo.attach(PIN_PWM_SERVO);
    use_sweep = false;
    delay(50);
    set_neutral();
}

void ServoManager::step_left() {
    if (pos_ix > 0) {
        pos_ix--;
    }
    servo.write(positions[pos_ix]);
    delay(50);
}

void ServoManager::step_right() {
    if (pos_ix < 11) {
        pos_ix++;
    }
    servo.write(positions[pos_ix]);
    delay(50);
}

void ServoManager::set_neutral() {
    pos_ix = SERVO_NUM_ANGLES / 2;
    servo.write(positions[pos_ix]);
    delay(100);
}

void ServoManager::sweep() {
    if (!use_sweep) {
        return;
    }

    if (pos_ix < pos_ix_previous) {
        pos_ix_previous = pos_ix;
        pos_ix--;
        if (pos_ix < 0) {
            pos_ix = 1;
        }
    }

    else {
        pos_ix_previous = pos_ix;
        pos_ix++;
        if (pos_ix == SERVO_NUM_ANGLES) {
            pos_ix = SERVO_NUM_ANGLES - 2;
        }
    }

    servo.write(positions[pos_ix]);
    delay(50);
}

void ServoManager::toggle_sweep() {
    use_sweep = !use_sweep;
}

char ServoManager::get_pos() {
    return pos_ix;
}
