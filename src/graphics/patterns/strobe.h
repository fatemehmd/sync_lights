#ifndef STROBE_H
#define STROBE_H

#include "pattern.h"
#include "graphics/state.h"

class Strobe : public Pattern
{
public:
    Strobe();
    void fill(uint8_t *output, long dt, State &state);
};

#endif