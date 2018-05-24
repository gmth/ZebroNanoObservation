#ifndef SERVOMANAGER_H
#define SERVOMANAGER_H

#include <Servo.h>
#include <Arduino.h>

#include "zebrobus.h"

#define SERVO_NUM_ANGLES 11


class ServoManager {
    private:
        /* Left to right */
        byte positions[SERVO_NUM_ANGLES] = {165, 150, 135, 120, 105, 90, 75, 60, 45, 30, 15};
        char pos_ix;                        // signed to identify < 0
        char pos_ix_previous;               // signed to identify < 0
        bool use_sweep;
        byte num_positions;
        byte *used_positions;
        byte pos_servo;
        Servo servo;

		ZebroBus *zebrobus;

    public:
        void init(byte *usedpositions, byte numpositions, ZebroBus *z);
        void step_left();
        void step_right();
        void set_neutral();
        void sweep();
        void sweep_toggle();
        void sweep_on();
        void sweep_off();
        char get_pos();
};
#endif
