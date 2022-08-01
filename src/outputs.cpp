#include "outputs.h"

Outputs::Outputs(State *state)
{
    __state = state;

    FastLED.addLeds<NEOPIXEL, 26>(ws2811, MAX_STRIP_LENGTH); // M5Stack Gray

    bufferWritePointer = 0;

    FastLED.setBrightness(255);
}

void Outputs::sampleTo(CRGB *source, CRGB *target, int targetCount)
{
    for (int i = 0; i < targetCount; i++)
    {
        double left, right, frac;
        double idx = i * (double)MAX_STRIP_LENGTH / (double)targetCount;
        modf(idx, &left);
        right = left + 1;
        frac = idx - left;

        CRGB col = source[(int)left];
        blend(col, source[(int)right], 1 - frac);

        target[i] = col;
    }

    for (int i = targetCount; i < MAX_STRIP_LENGTH; i++)
    {
        target[i] = CRGB::Black;
    }
}

void Outputs::display(CRGB *leds, CRGB *previewLeds)
{
    // ws2811 output. right now hardcoded to be delay on first two strips, but with sampling.
    for (int stripIdx = 0; stripIdx < NUM_STRIPS; stripIdx++) {
        // determine which slot to load delay from
        int bufferReadPointer = bufferWritePointer - 16 * stripIdx;
        if (bufferReadPointer < 0) 
            bufferReadPointer += BUFFER_SIZE;
        
        int length = constrain(__state->physicalStripParam(PhysicalStripParams::Length, stripIdx), MIN_STRIP_LENGTH, MAX_STRIP_LENGTH);
        
        // temporary, until we configure preview strip
        if (stripIdx == 1 && __state->globalParam(GlobalParams::PreviewNode) != 0) {
            sampleTo(previewLeds, ws2811 + stripIdx * MAX_STRIP_LENGTH, length);
            ws2811[stripIdx * MAX_STRIP_LENGTH] = CRGB::Red;
        }

        // color order correction
        if (__state->physicalStripParam(PhysicalStripParams::ColorOrder, stripIdx) != GRB)
        for (int i = 0; i < MAX_STRIP_LENGTH; i++){
            CRGB col = ws2811[stripIdx * MAX_STRIP_LENGTH + i];
            ws2811[stripIdx * MAX_STRIP_LENGTH + i] = CRGB(col.green, col.red, col.blue);
        }

        // strip direction
        if (__state->physicalStripParam(PhysicalStripParams::Direction, stripIdx) != 0){
            CRGB temp[length];
            memcpy(temp, ws2811 + stripIdx * MAX_STRIP_LENGTH, length * sizeof(CRGB));
            for (int i = 0; i < length; i++){
                ws2811[stripIdx * MAX_STRIP_LENGTH + i] = temp[length - 1 - i];
            }
        }
    }

    // inc circular buffer index;
    bufferWritePointer = (bufferWritePointer + 1) % BUFFER_SIZE;
    FastLED.show();
}