#include "cloud.h"

Cloud::Cloud() : Pattern("Cloud")
{
    pMetadata[0] = {NORMAL, 152, "DFT"};
    pMetadata[1] = {OCTAVE, 96, "SPD"};
    pMetadata[2] = {NORMAL, 200, "GAN"}; // gain
    pMetadata[3] = {NORMAL, 64, "DET"};  // noise octaves = detail

    pos = 0;
    _t = 0;
}

float Cloud::fractalNoise(uint16_t x, uint16_t y, float octave)
{
    float r = 0;
    float amp = 1.0;
    float scale = 0;

    float octWhole = floorf(octave);
    float octFrac = octave - octWhole;

    for (int oct = 0; oct < octWhole; oct++)
    {
        r += inoise8(x, y) * amp;
        scale += amp;
        amp /= 2;
        x *= 2;
        y *= 2;
    }

    r += inoise8(x, y) * amp * octFrac;
    scale += amp * octFrac;

    return constrain(r / scale, 0, 255);
}

void Cloud::fill(uint8_t *output, long dt, State &state)
{
    float up = (patternParam(state, 0) - 128) / 32.0;
    float octaves = (patternParam(state, 3)) / 32.0 + 1;
    float gain = patternParam(state, 2) / 127.0;

    _t += octave(state, 1) * dt;
    pos += dt * up;

    for (int i = 0; i < STRAND_LENGTH; i++)
    {
        float noise = fractalNoise(_t / 8, (1000 + i - pos / 32) * 10, octaves);
        int value = constrain(noise * gain, 0, 255);
        output[i] = value;
    }
}
