#include "patterns.h"

#include "esp_log.h"
#include <Arduino.h>
#include "FastLed.h"

namespace backpack {
Patterns::Patterns()
{

}

void Patterns::Setup()
{
    FastLED.addLeds<NEOPIXEL, 26>(leds, 60); // M5Stack Gray
}

void Patterns::RunPattern()
{
    switch (pattern_id)
    {
    case PatternId::kNone:
        Reset();
        break;
    case PatternId::kRainbow:
        Rainbow();
        break;
    case PatternId::kSolid:
        Solid();
        break;
    default:
        Reset();
        break;
    }
}
void Patterns::Rainbow()
{
    if (255 == hue)
        hue = 0;
    else
        FastLED.showColor(CHSV(hue++, 255, 50));
}
void Patterns::Reset()
{
}
void Patterns::Solid()
{
    FastLED.showColor(CHSV(166, 255, 50));
}
Patterns::~Patterns()
{
}

void Patterns::TogglePattern() {
    int pattern_size =  static_cast<int>(PatternId::kSize);
    pattern_num = (pattern_num + 1) % pattern_size;
    pattern_id = static_cast<PatternId>(pattern_num);
    ESP_LOGI("pattern", "pattern num is %d %d", pattern_num, pattern_size);

}

}  // namespace backpack