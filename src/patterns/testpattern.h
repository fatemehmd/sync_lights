#ifndef TESTPATTERN_H
#define TESTPATTERN_H

#include "pattern.h"
#include "../state.h"

class TestPattern : public Pattern
{
public:
    TestPattern();
    void fill(uint8_t *output, long dt, State &state);
};

#endif