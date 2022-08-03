#ifndef SPARKLE_H
#define SPARKLE_H

#include "pattern.h"
#include "graphics/state.h"

class Sparkle : public Pattern
{
public:
    Sparkle();
    void fill(uint8_t *output, long dt, State &state);
private:
    int timeSinceLastFade;
    int timeSinceLastSpark;
    CRGB _leds[STRAND_LENGTH];
    uint8_t heat[STRAND_LENGTH];
};

#endif