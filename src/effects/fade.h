#ifndef FADE_H
#define FADE_H

#include "../state.h"

class Fade
{
public:
    Fade(uint8_t bankIdx);
    void process(CRGB *dest, CRGB *source, long numLEDs, State &state);

private:
    CRGB buffer[STRAND_LENGTH];
    uint8_t _bankIdx;
};

#endif