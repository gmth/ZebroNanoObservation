#include <Servo.h>
#include <Arduino.h>


#define DISTANCE_NUM_ANGLES 11

class ServoManager {
    private:
        /* Left to right */
        byte positions[DISTANCE_NUM_ANGLES] = {165, 150, 135, 120, 105, 90, 75, 60, 45, 30, 15};
        char pos_ix = DISTANCE_NUM_ANGLES / 2;        // signed to identify < 0
        char pos_ix_previous = pos_ix;              // signed to identify < 0
        bool use_sweep;
        byte pos_servo;
        Servo servo;

    public:
        void init();
        void step_left();
        void step_right();
        void set_neutral();
        void sweep();
        void toggle_sweep();
        char get_pos();
};
