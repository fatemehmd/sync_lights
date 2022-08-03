#ifndef SCARF_H
#define SCARF_H

#include "pattern.h"
#include "graphics/state.h"

#define HUE_START .5
#define HUE_END .75
#define SATURATION .8

/**
    Pattern definition. The program cycles through a range on the wheel, and
    back again. This defines the boundaries. Note that wraparound for the full
    rainbow is not enabled. Would take special case code.
*/

class Scarf : public Pattern
{
public:
    Scarf();
    void fill(uint8_t *outputs, long dt, State &state); 

private:
    long pos;
    byte getClock(unsigned long mil, byte rate);
    byte modDist(byte x, byte y);
};

#endif