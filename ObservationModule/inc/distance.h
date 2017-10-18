#ifndef DISTANCE_H
#define DISTANCE_H
#include "Arduino.h"

/* Any read duration value lower than 200 means a distance of less than
 * 4 cm, which is way too close. These measurements are often erroneous and
 * should be ignored */
#define DISTANCE_DURATION_MIN 200

/* Any distance larger than the defined max is regarded infinetly far away */
#define DISTANCE_DURATION_MAX 6000                          // 1 meters one-way

#define MEASUREMENT_MAX_NUM 4
#define MEASUREMENT_MAX_NUM_TRIES 10

class Distance {
    private:
        // duration_latest is the raw measurement,
        // duration_previous is the previous measurement.
        // duration_estimate is the estimated duration based
        // on the other two. (finding the difference between 
        // infinity and a hw bug)
        unsigned int duration;
        unsigned int duration_prev;
        unsigned int duration_act;
        void clear_echo_pin();

    public:
        byte Distance::get_distance();
        byte Distance::get_smoothed_distance();
        byte Distance::get_max_distance();
        Distance();
};

#endif
