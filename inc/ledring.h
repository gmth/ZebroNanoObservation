#ifndef LEDRING_H
#define LEDRING_H

#include <FastLED.h>
#include <Arduino.h>
#include <math.h>

#define NUM_LEDS 24
#define ledsBrightness 30

class LedRing {
    private:
        byte *error;
        byte *distArray;
        byte max_distance;
        CRGB leds[NUM_LEDS];
        void radar_leds();

    public:
        LedRing(byte distArr[], byte max_dist);
        void init();
        void update_values();
        void set_led (byte hue, byte num);
};


#endif
