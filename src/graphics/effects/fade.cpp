#include "fade.h"

Fade::Fade(uint8_t bankIdx) : _bankIdx(bankIdx)
{
}

void Fade::process(CRGB *dest, CRGB *source, long numLEDs, State &state)
{
    uint8_t fadeAmount = state.patternParam(0, _bankIdx);
    uint8_t levelAmount = state.patternParam(1, _bankIdx);
    fadeToBlackBy(buffer, STRAND_LENGTH, fadeAmount);
    fadeToBlackBy(source, STRAND_LENGTH, levelAmount);

    // fadeToBlackBy(leds, STRAND_LENGTH, 224);
    for (int i = 0; i < numLEDs; i++){
        dest[i] = source[i] + buffer[i];
    }
    // nblend(leds, buffer, STRAND_LENGTH, 127);
    

    memcpy(buffer, dest, sizeof(buffer));
}