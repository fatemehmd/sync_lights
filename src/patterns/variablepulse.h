#ifndef VARIABLEPULSE_H
#define VARIABLEPULSE_H

#include "pattern.h"
#include "../state.h"

class VariablePulse : public Pattern
{
public:
    VariablePulse();
    void fill(uint8_t *output, long dt, State &state);

private:
    // return a cyclical (sine wave) value between min and max
    float cycle(float t, float period, float min, float max);
};

#endif