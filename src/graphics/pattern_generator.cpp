#include "pattern_generator.h"

Pattern *PatternGenerator::makePattern(uint8_t idx)
{
    switch (idx)
    {
    case 0:
        return new Cloud();
    case 1:
        return new AnalogFeedback();
    case 2:
        return new RainbowBlast();
    case 3:
        return new Particles();
    case 4:
        return new Fire2012();
    case 5:
        return new Scarf();
    case 6:
        return new Sparkle();
    case 7:
        return new Cleave();
    case 8:
        return new Strobe();
    case 9:
        return new Piano();
    default:
        return new TestPattern();
    }
}