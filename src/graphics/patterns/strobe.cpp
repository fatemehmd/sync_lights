#include "strobe.h"

Strobe::Strobe() : Pattern("Strobe")
{
    pMetadata[0] = {NORMAL, 128};
    strcpy(pMetadata[0].label, "LVL");
    pMetadata[1] = {DISCRETE, 0};
    strcpy(pMetadata[1].label, "WAV");
    pMetadata[2] = {OCTAVE, 127};
    strcpy(pMetadata[2].label, "RTE");
    pMetadata[3] = {NORMAL, 127};
    strcpy(pMetadata[3].label, "DTY");
    pMetadata[4] = {NORMAL, 127};
}

void Strobe::fill(uint8_t *output, long dt, State &state)
{
    unsigned long t = millis();

    float brightness = patternParam(state, 0) / 255.;
    float rate = octave(state, 2);
    uint8_t form = patternParam(state, 1) / 32;
    uint8_t dutyCycle = patternParam(state, 3);

    long adjustedT = rate * t;

    int timeInBeat = (adjustedT % PPQN) / (float)PPQN * 255;

    int wave;

    if (form == 0) // square wave
        wave = (timeInBeat < dutyCycle) ? 255 : 0;
    else if (form == 1) // sawtooth: not quite right
        wave = (timeInBeat < dutyCycle * 2) ? timeInBeat * 127. / dutyCycle : 0;
    else
        wave = sin8(timeInBeat);

    uint8_t level = wave * brightness;

    for (int i = 0; i < STRAND_LENGTH; i++)
    {
        output[i] = level;
    }
}