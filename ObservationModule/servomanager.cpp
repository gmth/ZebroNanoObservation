#include "inc/servomanager.h"
#include "inc/pinout.h"
#include "Arduino.h"

void ServoManager::init(byte *usedpositions, byte numpositions) {
    servo.attach(PIN_PWM_SERVO);
    use_sweep = true;
    used_positions = usedpositions;
    num_positions = numpositions;
    pos_ix = num_positions / 2;
    pos_ix_previous = pos_ix;
    delay(100);
    set_neutral();
}

void ServoManager::step_left() {
    if (pos_ix > 0) {
        pos_ix--;
    }
    servo.write(positions[used_positions[pos_ix]]);
    delay(70);
}

void ServoManager::step_right() {
    if (pos_ix < num_positions) {
        pos_ix++;
    }
    servo.write(positions[used_positions[pos_ix]]);
    delay(70);
}

void ServoManager::set_neutral() {
    pos_ix = num_positions / 2;
    servo.write(positions[used_positions[pos_ix]]);
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
        if (pos_ix == num_positions) {
            pos_ix = num_positions - 2;
        }
    }

    servo.write(positions[used_positions[pos_ix]]);
    delay(100);
}

void ServoManager::sweep_toggle() {
    use_sweep = !use_sweep;
}

void ServoManager::sweep_on() {
    use_sweep = true;
}

void ServoManager::sweep_off() {
    use_sweep = false;
}

char ServoManager::get_pos() {
    return used_positions[pos_ix];
}
