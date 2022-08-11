#ifndef RENDERER_H
#define RENDERER_H

#include <FastLED.h>

#include "state.h"
#include "patterns/pattern.h"


#include "effects/fade.h"



class Renderer
{
public:
    Renderer(State &state);
    void Render(State &state, unsigned long pulses);
    void CopyOutput(CRGB *mixerOutput, CRGB *previewOutput);
    void setPattern(int layerIdx, int patternIdx, Pattern* pattern);

private:
    Fade *fadeFX;
    Pattern **patterns = new Pattern *[NUM_LAYERS];

    CRGB layerOutput[NUM_LAYERS][STRAND_LENGTH];
    uint8_t layerOutput8bit[NUM_LAYERS][STRAND_LENGTH];
    CRGB mixerOutput[STRAND_LENGTH];
    CRGB previewOutput[STRAND_LENGTH];
};

#endif