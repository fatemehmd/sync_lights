#ifndef OUTPUTS_H
#define OUTPUTS_H

#include <FastLED.h>

#include "state.h"

#define BUFFER_SIZE 1024

class Outputs
{
public:
    Outputs(State *state);
    void display(CRGB *leds, CRGB *previewLeds);

private:
    void sampleTo(CRGB *source, CRGB *target, int targetCount);

    CRGB ws2811[NUM_STRIPS * MAX_STRIP_LENGTH];

    CRGB circularBuffer[BUFFER_SIZE][MAX_STRIP_LENGTH];


    int bufferWritePointer;

    State *__state;
};

#endif
