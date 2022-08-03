#include "scarf.h"

/**
 * Get a byte that cycles from 0-255, at a specified rate
 * typically, assign mil using mills();
 * rates, approximately (assuming 256ms in a second :P)
 * 8: 4hz
 * 7: 2hz
 * 6: 1hz
 * 5: 1/2hz
 * 4: 1/4hz
 * 3: 1/8hz
 * 2: 1/16hz
 * 1: 1/32hz
 */

byte Scarf::getClock(unsigned long mil, byte rate)
{
    return mil >> (8 - rate) % 256;
}

byte Scarf::modDist(byte x, byte y)
{
    return min(min(abs(x - y), abs(x - y + 256)), abs(x - y - 256));
}

Scarf::Scarf() : Pattern("Scarf")
{
    pos = 0;

    pMetadata[0] = {NORMAL, 128};
    pMetadata[1] = {OCTAVE, 128};
}

void Scarf::fill(uint8_t *outputs, long dt, State &state)
{
    unsigned long t = millis(); // hack
    t *= octave(state, 1);
    dt *= octave(state, 1);

    byte pulse = inoise8(t / 4.) * .5;
    byte drift = getClock(t, 3);
    pulse += drift;

    // pulse = getClock(t, 4) + 4 * sin((t * PI)/255.0);

    if (pulse > 255)
        pulse -= 255;

    for (byte pix = 0; pix < STRAND_LENGTH; pix++)
    {
        // location of the pixel on a 0-RENDER_RANGE scale.
        byte dist = pix * 255 / STRAND_LENGTH;

        byte delta = modDist(dist, pulse);
        // linear ramp up of brightness, for those within 1/8th of the reference point
        float value = max(255 - 6 * delta, 64);

        outputs[pix] = value;
    }
}