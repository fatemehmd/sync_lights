#ifndef RAINBOWBLAST_H
#define RAINBOWBLAST_H

#include "pattern.h"
#include "graphics/state.h"

class RainbowBlast : public Pattern
{
public:
    RainbowBlast();
    void fill(uint8_t* output, long dt, State &state);
private:
    long phaseDrift;
    long phaseDrift2;
    uint8_t wave8(uint8_t phase, uint8_t waveform);
};

#endif