#include "analogFeedback.h"

AnalogFeedback::AnalogFeedback() : Pattern("Analog2")
{
    pMetadata[0] = {NORMAL, 32, "FRQ"};
    pMetadata[1] = {NORMAL, 127, "SYN"};
    pMetadata[2] = {NORMAL, 0, "MOD"};
    pMetadata[3] = {NORMAL, 32, "SCL"};
    // pMetadata[4] = {NORMAL, 127, "OFF"};
    pMetadata[5] = {NORMAL, 127, "WAV"};

    phaseDrift = 0;

    for (int i = 0; i < STRAND_LENGTH; i++)
        frameBuffer[i] = 0;
}

uint8_t AnalogFeedback::wave8(uint8_t phase, uint8_t waveform)
{
    uint8_t ph = phase + 64;
    if (ph < waveform)
        return map(ph, 0, waveform, 0, 255);
    else
        return map(ph, waveform, 255, 255, 0);
}

void AnalogFeedback::fill(uint8_t *output, long dt, State &state)
{
    float freq = patternParam(state, 0) / 16.;
    float driftSpeed = (patternParam(state, 1) - 127) / 10.;
    float scale = patternParam(state, 3) / 32.;

    uint8_t phaseModDepth = patternParam(state, 2);
    uint8_t waveform = patternParam(state, 5);

    float syncCenter = STRAND_LENGTH / 2.;

    // in effect, we are using phaseDrift* as a fixed point number, with one decimal place
    phaseDrift += dt * driftSpeed;

    float phaseOffset = (phaseDrift % 2560) / 10.;
    for (int i = 0; i < STRAND_LENGTH; i++)
    {
        float phase = (i - syncCenter) * freq * 256 / STRAND_LENGTH;
        phase -= phaseOffset + frameBuffer[i] * phaseModDepth / 256.;

        phase = ((int)phase) % 256;
        if (phase < 0)
            phase += 256;

        frameBuffer[i] = wave8(phase, waveform);
    }

    // scale/wrap from frameBuffer to output
    for (int i = 0; i < STRAND_LENGTH; i++)
    {
        int16_t srcIndex = ((int)((i - syncCenter) * scale + syncCenter)) % STRAND_LENGTH;
        while (srcIndex < 0)
            srcIndex += STRAND_LENGTH;
        output[i] = frameBuffer[srcIndex];
    }

    // copy outback back into the frameBuffer
    for (int i = 0; i < STRAND_LENGTH; i++)
        frameBuffer[i] = output[i];
}