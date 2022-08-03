#ifndef CLEAVE_H
#define CLEAVE_H

#include "pattern.h"
#include "graphics/state.h"

class Cleave : public Pattern
{
public:
    Cleave();
    void fill(uint8_t *output, long dt, State &state);

private:
    float accRepeats;
    uint8_t values[STRAND_LENGTH];
};

#endif