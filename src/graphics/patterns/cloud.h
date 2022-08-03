#ifndef CLOUD_H
#define CLOUD_H

#include "pattern.h"
#include "graphics/state.h"

class Cloud : public Pattern
{
public:
    Cloud();
    void fill(uint8_t *output, long dt, State &state);

private:
    float fractalNoise(uint16_t x, uint16_t y, float octave);
    long pos, _t;
};

#endif