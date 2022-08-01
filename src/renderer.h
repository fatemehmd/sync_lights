#ifndef RENDERER_H
#define RENDERER_H

#include <FastLED.h>

#include "state.h"

#include "patterns/pattern.h"
#include "patterns/cloud.h"
#include "patterns/scarf.h"
#include "patterns/variablePulse.h"
#include "patterns/rainbowBlast.h"
#include "patterns/sparkle.h"
#include "patterns/testpattern.h"
#include "patterns/piano.h"
#include "patterns/fire2012.h"
#include "patterns/particles.h"
#include "patterns/cleave.h"
#include "patterns/strobe.h"
#include "patterns/analogFeedback.h"

#include "effects/fade.h"

class Renderer
{
public:
    Renderer(State &state);
    void Render(State &state, unsigned long pulses);
    void CopyOutput(CRGB *mixerOutput, CRGB *previewOutput);

private:
    Fade *fadeFX;
    Pattern **generators = new Pattern *[NUM_LAYERS];

    Pattern *makeGenerator(uint8_t idx);

    CRGB layerOutput[NUM_LAYERS][STRAND_LENGTH];
    uint8_t layerOutput8bit[NUM_LAYERS][STRAND_LENGTH];
    CRGB mixerOutput[STRAND_LENGTH];
    CRGB previewOutput[STRAND_LENGTH];
};

#endif