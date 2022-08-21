#include "rainbowblast.h"

RainbowBlast::RainbowBlast() : Pattern("Analog1")
{
    pMetadata[0] = {NORMAL, 32, "FQ1"};
    pMetadata[1] = {NORMAL, 127, "SY1"};
    pMetadata[2] = {NORMAL, 0, "MOD"};
    pMetadata[3] = {NORMAL, 32, "FQ2"};
    pMetadata[4] = {NORMAL, 127, "SY2"};
    pMetadata[5] = {NORMAL, 127, "WAV"};

    phaseDrift = 0;
    phaseDrift2 = 0;
}

uint8_t RainbowBlast::wave8(uint8_t phase, uint8_t waveform)
{
    uint8_t ph = phase + 64;
    if (ph < waveform)
        return map(ph, 0, waveform, 0, 255);
    else
        return map(ph, waveform, 255, 255, 0);
}

void RainbowBlast::fill(uint8_t *output, long dt, State &state)
{
    float freq = patternParam(state, 0) / 16.;
    float driftSpeed = (patternParam(state, 1) - 127) / 10.;
    float freq2 = patternParam(state, 3) / 16.;
    float syncDrift = (patternParam(state, 4) - 127) / 10.;
    uint8_t phaseModDepth = patternParam(state, 2);
    uint8_t waveform = patternParam(state, 5);

    float syncCenter = STRAND_LENGTH / 2;

    // in effect, we are using phaseDrift* as a fixed point number, with one decimal place
    phaseDrift += dt * driftSpeed;
    phaseDrift2 += dt * syncDrift;

    float phaseOffset = (phaseDrift % 2560) / 10.;
    float phaseOffset2 = (phaseDrift2 % 2560) / 10.;
    for (int i = 0; i < STRAND_LENGTH; i++)
    {
        float phase = (i - syncCenter) * freq * 256 / STRAND_LENGTH;
        float phase2 = (i - syncCenter) * freq2 * 256 / STRAND_LENGTH;
        phase2 -= (phaseOffset2 + phaseOffset);
        float phaseMod = sinf(phase2 / 256 * 2 * PI) * phaseModDepth;
        phase += phaseMod - phaseOffset;

        phase = ((int)phase) % 256;
        if (phase < 0)
            phase += 256;

        output[i] = wave8(phase, waveform);
    }
}