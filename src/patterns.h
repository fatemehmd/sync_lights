#ifndef PATTERNS_H
#define PATTERNS_H

#include <Arduino.h>
#include "FastLed.h"

#define NUM_LEDS  60

namespace backpack {



enum class PatternId
{
    kNone = 0,
    kRainbow,
    kSolid,
    kCylon,
    kSize,
};

class Patterns
{
CRGB leds[NUM_LEDS];
public:
    Patterns();
    ~Patterns() {};
    void RunPattern();
    void Setup();
    void TogglePattern();

private:
    uint8_t hue = 0;
    int pattern_num = 0;
    int pattern_size = 0;
    PatternId pattern_id = PatternId::kNone;
    void Reset();
    void Rainbow();
    void Cylon();
    void Solid();
    void FadeAll();

};
}  // namespace backpack
#endif
