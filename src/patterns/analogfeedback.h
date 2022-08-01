#ifndef ANALOGFEEDBACK_H
#define ANALOGFEEDBACK_H

#include "pattern.h"
#include "../state.h"

class AnalogFeedback : public Pattern
{
public:
    AnalogFeedback();
    void fill(uint8_t* output, long dt, State &state);
private:
    long phaseDrift;
    long phaseDrift2;

    uint8_t frameBuffer[STRAND_LENGTH];
    uint8_t wave8(uint8_t phase, uint8_t waveform);
};

#endif