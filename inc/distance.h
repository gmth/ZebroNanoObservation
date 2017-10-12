#ifndef DISTANCE_H
#define DISTANCE_H
#include "Arduino.h"

/* Any read duration value lower than 200 means a distance of less than
 * 4 cm, which is way too close. These measurements are often erroneous and
 * should be ignored */
#define DISTANCE_DURATION_MIN 200

/* Any distance larger than the defined max is regarded infinetly far away */
#define DISTANCE_DURATION_MAX 10000         // ~1.7 meters one-way

#define MEASUREMENT_MAX_NUM 4
#define MEASUREMENT_MAX_NUM_TRIES 10

class Distance {
    private:
        unsigned int echo_start;
        unsigned int echo_end;
        
        // duration_latest is the raw measurement,
        // duration_previous is the previous measurement.
        // duration_estimate is the estimated duration based
        // on the other two. (finding the difference between 
        // infinity and a hw bug)
        unsigned int duration_latest;
        unsigned int duration_previous;
        unsigned int duration_estimate;
        
        unsigned int durations[MEASUREMENT_MAX_NUM];
        unsigned int num_failed_readings;
        bool new_data_available;
        bool triggered;

    public:
        bool new_distance_available;
        void trigger();
        void IRQ_on_echo();
        int check_distance(byte measurement_ix);
        void rotate_left();
        void rotate_right();
        byte get_distance_cm(byte num_measurements);
        int get_duration();
        byte get_max_distance();
        Distance();
};

#endif
