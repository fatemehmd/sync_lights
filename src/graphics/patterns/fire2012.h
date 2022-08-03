#ifndef FIRE2012_H
#define FIRE2012_H

#include "pattern.h"
#include "graphics/state.h"

class Fire2012 : public Pattern
{
public:
    Fire2012();
    void fill(uint8_t *output, long dt, State &state);

private:
    void runFireSim(State &state);
    // Array of temperature readings at each simulation cell
    byte heat[STRAND_LENGTH];
    CRGB _leds[STRAND_LENGTH];
    int timeSinceLastFrame;
};

#endif