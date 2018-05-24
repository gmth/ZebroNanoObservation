#include "inc/servomanager.h"
#include "inc/pinout.h"
#include "inc/uart.h"
#include "Arduino.h"

void ServoManager::init(byte *usedpositions, byte numpositions, ZebroBus *z) {
    servo.attach(PIN_PWM_SERVO);
    used_positions = usedpositions;
    num_positions = numpositions;
    pos_ix = num_positions / 2;
    pos_ix_previous = pos_ix;
	
	zebrobus = z;
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
	bool moved = false;
    if (zebrobus->read(VREGS_OBS_SERVO_NOSWEEP)) {

		if (zebrobus->read(VREGS_OBS_SERVO_SET0)) {
			pos_ix = 0;
			moved = true;
		}

		if (zebrobus->read(VREGS_OBS_SERVO_SET1)) {
			pos_ix = 1;
			moved = true;
		}

		if (zebrobus->read(VREGS_OBS_SERVO_SET2)) {
			pos_ix = 2;
			moved = true;
		}

		if (zebrobus->read(VREGS_OBS_SERVO_SET3)) {
			pos_ix = 3;
			moved = true;
		}
		
		if (zebrobus->read(VREGS_OBS_SERVO_SET4)) {
			pos_ix = 4;
			moved = true;
		}

		if (moved) {
			delay(100);
		}

    } else {
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
	}
}


char ServoManager::get_pos() {
    return used_positions[pos_ix];
}
