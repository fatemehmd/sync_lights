#ifndef PATTERNS_H
#define PATTERNS_H

#include <Arduino.h>
#include "FastLed.h"

namespace backpack {


enum class PatternId
{
    kNone = 0,
    kRainbow,
    kSolid,
    kSize,
};

class Patterns
{
CRGB leds[60];
public:
    Patterns();
    ~Patterns();
    void RunPattern();
    void Setup();
    void TogglePattern();

private:
    uint8_t hue = 0;
    int pattern_num = 0;
    PatternId pattern_id = PatternId::kNone;
    void Reset();
    void Rainbow();
    void Solid();

};
}  // namespace backpack
#endif
