#include "testpattern.h"

TestPattern::TestPattern() : Pattern("Test")
{
}

void TestPattern::fill(uint8_t *output, long dt, State &state)
{
    for (int i = 0; i < STRAND_LENGTH; i++)
        output[i] = map(i, 0, STRAND_LENGTH - 1, 0, 255);
}