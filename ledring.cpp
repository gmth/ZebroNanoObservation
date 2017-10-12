#include <math.h>
#include "./inc/servomanager.h"
#include "./inc/pinout.h"
#include "./inc/ledring.h"

#define NUM_LEDS 24
#define ledsBrightness 30

void LedRing::radar_leds() {
    for (int i = 0; i < SERVO_NUM_ANGLES; i++) {
        int color = 100*distArray[i] / 170; //Takes elements 3, 16, 29, 42, 55, ..., 146
        //color is red (0) at distance of zero, green (90) at CONFIG.getmaxDistance() or higher
        leds[SERVO_NUM_ANGLES-1 -i] = CHSV(color,255,255);
    }
}

LedRing::LedRing(byte distArr[], byte max_dist){
    distArray = distArr;
    max_distance = max_dist;
}

void LedRing::init() {
    Serial.println("--> LEDs");
    Serial.println("\taddLeds");
    LEDS.addLeds<WS2812, PIN_DATA_LED, GRB>(leds, NUM_LEDS);
    Serial.println("\tsetBrightness");
    LEDS.setBrightness(ledsBrightness);
    Serial.println("\tTest");
    update_values();
}

void LedRing::update_values(){
    FastLED.clear();
    radar_leds();
    FastLED.show();
}

void LedRing::set_led (byte hue, byte num){
    if(hue){
        leds[num] = CHSV(hue,255,255);
    }
    else{
        leds[num] = CRGB::Black;
    }
    FastLED.show();
}
