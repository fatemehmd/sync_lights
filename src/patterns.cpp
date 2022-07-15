#include "patterns.h"

#include "esp_log.h"
#include <Arduino.h>
#include "FastLed.h"



namespace backpack {

Patterns::Patterns() {    
    pattern_size = (int) PatternId::kSize;
};

void Patterns::FadeAll() { 
    for(int i = 0; i < NUM_LEDS; i++) 
    { 
        leds[i].nscale8(250); 
    } 
}

void Patterns::Setup()
{
    FastLED.addLeds<NEOPIXEL, 26>(leds, NUM_LEDS); // M5Stack Gray
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
    case PatternId::kCylon:
        Cylon();
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

void Patterns::Cylon() {

	// First slide the led in one direction
	for(int i = 0; i < NUM_LEDS; i++) {
		// Set the i'th led to red 
		leds[i] = CHSV(hue++, 255, 255);
		// Show the leds
		FastLED.show(); 
		// now that we've shown the leds, reset the i'th led to black
		// leds[i] = CRGB::Black;
		FadeAll();
		// Wait a little bit before we loop around and do it again
		delay(10);
	}

	// Now go in the other direction.  
	for(int i = (NUM_LEDS)-1; i >= 0; i--) {
		// Set the i'th led to red 
		leds[i] = CHSV(hue++, 255, 255);
		// Show the leds
		FastLED.show();
		// now that we've shown the leds, reset the i'th led to black
		// leds[i] = CRGB::Black;
		FadeAll();
		// Wait a little bit before we loop around and do it again
		delay(10);
	}
}

void Patterns::Reset()
{
    FadeAll();
}

void Patterns::Solid()
{
    FastLED.showColor(CHSV(166, 255, 50));
}


void Patterns::TogglePattern() {
    pattern_num = (pattern_num + 1) % pattern_size;
    pattern_id = static_cast<PatternId>(pattern_num);
    ESP_LOGI("pattern", "pattern num is %d %d", pattern_num, pattern_size);
}

}  // namespace backpack