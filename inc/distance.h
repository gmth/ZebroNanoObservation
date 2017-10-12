#ifndef DISTANCE_H
#define DISTANCE_H
#include "Arduino.h"

/* Any read duration value lower than 200 means a distance of less than
 * 3.5 cm, which is way too close. These measurements are often erroneous and
 * should be ignored */
#define DISTANCE_DURATION_MIN 200

/* Any distance larger than the defined max is regarded infinetly far away */
#define DISTANCE_DURATION_MAX 10000

#define MEASUREMENT_MAX_NUM 4
#define MEASUREMENT_MAX_NUM_TRIES 10

class Distance {
    private:
        unsigned int echo_start;
        unsigned int echo_end;
        unsigned int duration_latest;
        unsigned int durations[MEASUREMENT_MAX_NUM];
        unsigned int num_failed_readings;
        bool new_data_available;
        bool triggered;

    public:
        bool new_distance_available;
        int trigger();
        void IRQ_on_echo();
        int check_distance(byte measurement_ix);
        void rotate_left();
        void rotate_right();
        byte get_distance_cm(byte num_measurements);
        int get_duration();
        Distance();
};

#endif
