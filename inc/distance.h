#ifndef DISTANCE_H
#define DISTANCE_H
#include "Arduino.h"

/* Any read duration value lower than 200 means a distance of less than
 * 3.5 cm, which is way too close. These measurements are often erroneous and
 * should be ignored */
#define DISTANCE_DURATION_MIN 200

/* Any distance larger than the defined max is regarded infinetly far away */
#define DISTANCE_DURATION_MAX 5000

class Distance {
    private:
        unsigned int echo_start;
        unsigned int echo_end;
        unsigned int duration_latest;
        unsigned int durations[4];
        uint8_t ix_duration;
        bool new_data_available;
        bool triggered;

    public:
        bool new_distance_available;
        void trigger();
        void IRQ_on_echo();
        void update_buf();
        void rotate_left();
        void rotate_right();
        void reset();
        int get_distance_mm();
        int get_duration();
        Distance();
};


#endif
